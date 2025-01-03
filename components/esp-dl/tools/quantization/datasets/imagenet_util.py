import time
from typing import Callable

import onnxruntime
import pandas as pd
import torch
import torch.utils.data
import torchvision.datasets as datasets
import torchvision.transforms as transforms
from ppq.executor.torch import TorchExecutor
from ppq.IR import BaseGraph
from torch.utils.data.dataloader import DataLoader
from torch.utils.data.dataset import Subset
from tqdm import tqdm


def accuracy(output, target, topk=(1,)):
    """Computes the precision@k for the specified values of k
    prec1, prec5 = accuracy(output.data, target, topk=(1, 5))
    """
    maxk = max(topk)
    batch_size = target.size(0)

    _, pred = output.topk(maxk, 1, True, True)
    pred = pred.t()
    correct = pred.eq(target.view(1, -1).expand_as(pred))

    res = []
    for k in topk:
        correct_k = correct[:k].reshape(-1).float().sum(0, keepdim=True)
        res.append(correct_k.mul_(100.0 / batch_size))
    return res


def load_imagenet_from_directory(
    directory: str,
    subset: int = None,
    batchsize: int = 32,
    shuffle: bool = False,
    require_label: bool = True,
    num_of_workers: int = 12,
) -> torch.utils.data.DataLoader:
    """
    A standardized Imagenet data loading process,
    directory: The location where the data is loaded
    subset: If set to a non-empty value, a subset of the specified size is extracted from the dataset
    batchsize: The batch size of the data loader
    require_label: Whether labels are required
    shuffle: Whether to shuffle the dataset
    """
    dataset = datasets.ImageFolder(
        directory,
        transforms.Compose(
            [
                transforms.Resize(256),
                transforms.CenterCrop(224),
                transforms.ToTensor(),
                transforms.Normalize(
                    mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]
                ),
            ]
        ),
    )

    if subset:
        dataset = Subset(dataset, indices=[_ for _ in range(0, subset)])
    if require_label:
        return torch.utils.data.DataLoader(
            dataset=dataset,
            batch_size=batchsize,
            shuffle=shuffle,
            num_workers=num_of_workers,
            pin_memory=False,
            drop_last=True,  # onnx 模型不支持动态 batchsize，最后一个批次的数据尺寸可能不对齐，因此丢掉最后一个批次的数据
        )
    else:
        return torch.utils.data.DataLoader(
            dataset=dataset,
            batch_size=batchsize,
            shuffle=shuffle,
            num_workers=num_of_workers,
            pin_memory=False,
            collate_fn=lambda x: torch.cat(
                [sample[0].unsqueeze(0) for sample in x], dim=0
            ),
            drop_last=False,  # 不需要标签的数据为 calib 数据，无需 drop
        )


def evaluate_torch_module_with_imagenet(
    model: torch.nn.Module,
    imagenet_validation_dir: str = None,
    batchsize: int = 32,
    device: str = "cuda",
    imagenet_validation_loader: DataLoader = None,
    verbose: bool = True,
) -> pd.DataFrame:
    model.eval()
    with torch.no_grad():
        model_forward_function = lambda input_tensor: model(input_tensor)
        return _evaluate_any_module_with_imagenet(
            model_forward_function=model_forward_function,
            batchsize=batchsize,
            device=device,
            imagenet_validation_dir=imagenet_validation_dir,
            imagenet_validation_loader=imagenet_validation_loader,
            verbose=verbose,
        )


def evaluate_onnx_module_with_imagenet(
    onnxruntime_model_path: str,
    imagenet_validation_dir: str = None,
    batchsize: int = 32,
    device: str = "cuda",
    imagenet_validation_loader: DataLoader = None,
    verbose: bool = True,
) -> pd.DataFrame:
    sess = onnxruntime.InferenceSession(
        path_or_bytes=onnxruntime_model_path, providers=["CUDAExecutionProvider"]
    )
    input_placeholder_name = sess.get_inputs()[0].name
    with torch.no_grad():
        # TODO 可能导致一些内存资源浪费
        model_forward_function = lambda input_tensor: torch.tensor(
            sess.run(
                input_feed={input_placeholder_name: input_tensor.cpu().numpy()},
                output_names=None,
            )
        )[0]
        return _evaluate_any_module_with_imagenet(
            model_forward_function=model_forward_function,
            batchsize=batchsize,
            device=device,
            imagenet_validation_dir=imagenet_validation_dir,
            imagenet_validation_loader=imagenet_validation_loader,
            verbose=verbose,
        )


def evaluate_mmlab_module_with_imagenet(
    model: torch.nn.Module,
    imagenet_validation_dir: str = None,
    batchsize: int = 32,
    device: str = "cuda",
    imagenet_validation_loader: DataLoader = None,
    verbose: bool = True,
) -> pd.DataFrame:
    model.eval()
    with torch.no_grad():
        model_forward_function = lambda input_tensor: model.forward_test(
            input_tensor, img_metas={}
        )
        return _evaluate_any_module_with_imagenet(
            model_forward_function=model_forward_function,
            batchsize=batchsize,
            device=device,
            imagenet_validation_dir=imagenet_validation_dir,
            imagenet_validation_loader=imagenet_validation_loader,
            verbose=verbose,
        )


def evaluate_ppq_module_with_imagenet(
    model: BaseGraph,
    imagenet_validation_dir: str = None,
    batchsize: int = 32,
    device: str = "cuda",
    imagenet_validation_loader: DataLoader = None,
    verbose: bool = True,
) -> pd.DataFrame:
    """
    一套用来测试 ppq 模块的逻辑，
    直接送入 ppq.IR.BaseGraph 就好了
    """

    executor = TorchExecutor(graph=model, device=device)
    model_forward_function = lambda input_tensor: torch.tensor(
        executor(*[input_tensor])[0]
    )
    return _evaluate_any_module_with_imagenet(
        model_forward_function=model_forward_function,
        batchsize=batchsize,
        device=device,
        imagenet_validation_dir=imagenet_validation_dir,
        imagenet_validation_loader=imagenet_validation_loader,
        verbose=verbose,
    )


def _evaluate_any_module_with_imagenet(
    model_forward_function: Callable,
    imagenet_validation_dir: str,
    batchsize: int = 32,
    device: str = "cuda",
    imagenet_validation_loader: DataLoader = None,
    verbose: bool = True,
):
    """
    一套十分标准的imagenet测试逻辑
    """

    recorder = {"loss": [], "top1_accuracy": [], "top5_accuracy": [], "batch_time": []}

    if imagenet_validation_loader is None:
        imagenet_validation_loader = load_imagenet_from_directory(
            imagenet_validation_dir, batchsize=batchsize, shuffle=False
        )

    loss_fn = torch.nn.CrossEntropyLoss().to("cpu")

    for batch_idx, (batch_input, batch_label) in tqdm(
        enumerate(imagenet_validation_loader),
        desc="Evaluating Model...",
        total=len(imagenet_validation_loader),
    ):
        batch_input = batch_input.to(device)
        batch_label = batch_label.to(device)
        batch_time_mark_point = time.time()

        batch_pred = model_forward_function(batch_input)
        if isinstance(batch_pred, list):
            batch_pred = torch.tensor(batch_pred)

        recorder["batch_time"].append(time.time() - batch_time_mark_point)
        recorder["loss"].append(loss_fn(batch_pred.to("cpu"), batch_label.to("cpu")))
        prec1, prec5 = accuracy(
            torch.tensor(batch_pred).to("cpu"), batch_label.to("cpu"), topk=(1, 5)
        )
        recorder["top1_accuracy"].append(prec1.item())
        recorder["top5_accuracy"].append(prec5.item())

        if batch_idx % 100 == 0 and verbose:
            print(
                "Test: [{0} / {1}]\t"
                "Prec@1 {top1:.3f} ({top1:.3f})\t"
                "Prec@5 {top5:.3f} ({top5:.3f})".format(
                    batch_idx,
                    len(imagenet_validation_loader),
                    top1=sum(recorder["top1_accuracy"])
                    / len(recorder["top1_accuracy"]),
                    top5=sum(recorder["top5_accuracy"])
                    / len(recorder["top5_accuracy"]),
                )
            )

    if verbose:
        print(
            " * Prec@1 {top1:.3f} Prec@5 {top5:.3f}".format(
                top1=sum(recorder["top1_accuracy"]) / len(recorder["top1_accuracy"]),
                top5=sum(recorder["top5_accuracy"]) / len(recorder["top5_accuracy"]),
            )
        )

    # dump records toward dataframe
    dataframe = pd.DataFrame()
    for column_name in recorder:
        dataframe[column_name] = recorder[column_name]
    return dataframe
