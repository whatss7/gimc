# GIMC

简易原神材料合成计算器。

[p-to 模式的网页版](https://whatss7.github.io/gimc-web/)

## Building

```
cmake . -B build
cmake --build build
```

## Usage

直接运行 gimc.exe 即可。以下展示了一个交互的例子，其中计算得到以下数据：

- 拥有 7 个蓝色材料、14 个绿色材料、28 个白色材料时，全部合成为蓝色材料时：
  - 不使用相关角色，可以合成得到 14 个蓝色材料；
  - 使用 25% 返还的角色，期望获得 15.24 个蓝色材料，获得 16 个蓝色材料的概率为 27.70%，理论最多可获得 20 个蓝色材料；
  - 使用 10% 双倍的角色，期望获得 15.41 个蓝色材料，获得 16 个蓝色材料的概率为 43.15%，理论最多可获得 27 个蓝色材料。
- 拥有 7 个蓝色材料、16 个绿色材料、30 个白色材料时，全部合成为蓝色材料
  - 不使用相关角色，可以合成得到 15 个蓝色材料；
  - 使用 25% 返还的角色，期望获得 16.25 个蓝色材料，获得 16 个蓝色材料的概率为 94.85%，理论最多可获得 21 个蓝色材料；
  - 使用 10% 双倍的角色，期望获得 16.56 个蓝色材料，获得 16 个蓝色材料的概率为 86.70%，理论最多可获得 31 个蓝色材料。

```
>>> pto 3 16
Mode changed from simulate to p-to 3 16

>>> 7 14 28
 Sequence: 7 14 28
CM/TM/3rd: 14-15.24/27.70%(20)-15.41/43.15%(27)

>>> 7 16 30
 Sequence: 7 16 30
CM/TM/3rd: 15-16.25/94.85%(21)-16.56/86.70%(31)
```

更多用法请在交互界面输入 help

（原谅我的垃圾英语（编程的时候不想切中文输入法了