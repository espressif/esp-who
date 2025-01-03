| Supported Targets | ESP32-S3 | ESP32-P4 |
| ----------------- | -------- | -------- |


# Pedestrian Detect Example

# Example Output

After the flashing you should see the output at idf monitor:

```
I (1566) dl::Model: model:torch-jit-export, version:0

I (1566) dl::Model: Conv_0: Conv
I (1566) dl::Model: Conv_2: Conv
I (1566) dl::Model: Conv_4: Conv
I (1576) dl::Model: Conv_6: Conv
I (1576) dl::Model: Conv_8: Conv
I (1586) dl::Model: Conv_10: Conv
I (1586) dl::Model: Conv_12: Conv
I (1586) dl::Model: Conv_14: Conv
I (1596) dl::Model: Conv_16: Conv
I (1596) dl::Model: Conv_18: Conv
I (1606) dl::Model: Conv_20: Conv
I (1606) dl::Model: Conv_22: Conv
I (1606) dl::Model: Conv_66: Conv
I (1616) dl::Model: Conv_24: Conv
I (1616) dl::Model: Conv_26: Conv
I (1626) dl::Model: Conv_28: Conv
I (1626) dl::Model: Conv_30: Conv
I (1626) dl::Model: Conv_32: Conv
I (1636) dl::Model: Conv_34: Conv
I (1636) dl::Model: Conv_36: Conv
I (1646) dl::Model: Conv_38: Conv
I (1646) dl::Model: Conv_40: Conv
I (1646) dl::Model: Conv_42: Conv
I (1656) dl::Model: Conv_44: Conv
I (1656) dl::Model: Conv_46: Conv
I (1666) dl::Model: Conv_68: Conv
I (1666) dl::Model: GlobalAveragePool_48: GlobalAveragePool
I (1676) dl::Model: Conv_49: Conv
I (1676) dl::Model: Conv_51: Conv
I (1676) dl::Model: Sigmoid_52: Sigmoid
I (1686) dl::Model: Mul_53: Mul
I (1686) dl::Model: Conv_54: Conv
I (1696) dl::Model: Conv_56: Conv
I (1696) dl::Model: GlobalAveragePool_58: GlobalAveragePool
I (1706) dl::Model: Conv_59: Conv
I (1706) dl::Model: Conv_61: Conv
I (1706) dl::Model: Sigmoid_62: Sigmoid
I (1716) dl::Model: Mul_63: Mul
I (1716) dl::Model: Conv_64: Conv
I (1726) dl::Model: Conv_70: Conv
I (1726) dl::Model: Resize_72: Resize
I (1736) dl::Model: Concat_73: Concat
I (1736) dl::Model: Conv_74: Conv
I (1736) dl::Model: Conv_76: Conv
I (1746) dl::Model: Conv_78: Conv
I (1746) dl::Model: Conv_80: Conv
I (1756) dl::Model: Resize_82: Resize
I (1756) dl::Model: Concat_83: Concat
I (1756) dl::Model: Conv_84: Conv
I (1766) dl::Model: Conv_86: Conv
I (1766) dl::Model: Conv_88: Conv
I (1776) dl::Model: Conv_90: Conv
I (1776) dl::Model: Conv_92: Conv
I (1776) dl::Model: Conv_118: Conv
I (1786) dl::Model: Conv_94: Conv
I (1786) dl::Model: Conv_120: Conv
I (1796) dl::Model: Concat_96: Concat
I (1796) dl::Model: Conv_122: Conv
I (1796) dl::Model: Conv_97: Conv
I (1806) dl::Model: Conv_124: Conv
I (1806) dl::Model: Conv_99: Conv
I (1816) dl::Model: GlobalAveragePool_126: GlobalAveragePool
I (1816) dl::Model: Conv_135: Conv
I (1826) dl::Model: Conv_101: Conv
I (1826) dl::Model: Conv_127: Conv
I (1836) dl::Model: Conv_137: Conv
I (1836) dl::Model: Conv_103: Conv
I (1836) dl::Model: Sigmoid_128: Sigmoid
I (1846) dl::Model: Sigmoid_138: Sigmoid
I (1846) dl::Model: Conv_105: Conv
I (1856) dl::Model: Conv_140: Conv
I (1856) dl::Model: Mul_129: Mul
I (1856) dl::Model: Conv_107: Conv
I (1866) dl::Model: Conv_142: Conv
I (1866) dl::Model: Conv_130: Conv
I (1876) dl::Model: Concat_109: Concat
I (1876) dl::Model: Conv_144: Conv
I (1886) dl::Model: Conv_132: Conv
I (1886) dl::Model: Conv_133: Conv
I (1886) dl::Model: Conv_110: Conv
I (1896) dl::Model: Conv_146: Conv
I (1896) dl::Model: Sigmoid_134: Sigmoid
I (1906) dl::Model: Conv_112: Conv
I (1906) dl::Model: GlobalAveragePool_148: GlobalAveragePool
I (1916) dl::Model: Conv_157: Conv
I (1916) dl::Model: Mul_139: Mul
I (1916) dl::Model: Conv_114: Conv
I (1926) dl::Model: Conv_149: Conv
I (1926) dl::Model: Conv_159: Conv
I (1936) dl::Model: Conv_116: Conv
I (1936) dl::Model: Sigmoid_150: Sigmoid
I (1946) dl::Model: Sigmoid_160: Sigmoid
I (1946) dl::Model: Conv_162: Conv
I (1946) dl::Model: Mul_151: Mul
I (1956) dl::Model: Conv_164: Conv
I (1956) dl::Model: Conv_152: Conv
I (1966) dl::Model: Conv_166: Conv
I (1966) dl::Model: Conv_154: Conv
I (1966) dl::Model: Conv_155: Conv
I (1976) dl::Model: Conv_168: Conv
I (1976) dl::Model: Sigmoid_156: Sigmoid
I (1986) dl::Model: GlobalAveragePool_170: GlobalAveragePool
I (1986) dl::Model: Conv_179: Conv
I (1996) dl::Model: Mul_161: Mul
I (1996) dl::Model: Conv_171: Conv
I (2006) dl::Model: Conv_181: Conv
I (2006) dl::Model: Sigmoid_172: Sigmoid
I (2006) dl::Model: Sigmoid_182: Sigmoid
I (2016) dl::Model: Mul_173: Mul
I (2016) dl::Model: Conv_174: Conv
I (2026) dl::Model: Conv_176: Conv
I (2026) dl::Model: Conv_177: Conv
I (2036) dl::Model: Sigmoid_178: Sigmoid
I (2036) dl::Model: Mul_183: Mul
I (2056) MemoryManagerGreedy: Maximum mermory size: 551936

I (2136) pedestrian_detect: [score: 0.883883, x1: 283, y1: 191, x2: 371, y2: 462]

I (2136) pedestrian_detect: [score: 0.870524, x1: 146, y1: 183, x2: 249, y2: 464]

I (2136) pedestrian_detect: [score: 0.755190, x1: 411, y1: 226, x2: 487, y2: 392]
```