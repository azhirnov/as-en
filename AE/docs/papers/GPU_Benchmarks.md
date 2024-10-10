GPUs:
* [Adreno 660](bench/Adreno_660.md)
* [Adreno 505](bench/Adreno_505.md)
* [AMD RX 570](bench/AMD_RX570.md)
* [Apple M1](bench/Apple_M1.md)
* [Intel UHD 620](bench/Intel_UHD620.md)
* [Mali G57](bench/ARM_Mali_G57.md)
* [Mali T830](bench/ARM_Mali_T830.md)
* [NVidia RTX 2080](bench/NVidia_RTX2080.md)
* [PowerVR BXM-8-256](bench/PowerVR_BXM.md)

Other:
* [Comparison of Results](#Comparison-of-Results)
* [Test Sources](#Test-Sources)


# Comparison of Results

## Subgroups

| GPU | subgroup size | helper invocation early termination | merge between instances | always full subgroup in FS |
|---|---|---|---|---|
| NV RTX 20xx           | 32     | no      | no      | no      |
| Adreno 6xx            | 64/128 | **yes** | **yes** | no      |
| Adreno 5xx            | ?      | -       | -       | ?       |
| AMD GCN4              | 64     |         |         |         |
| Apple M1              |        |         |         |         |
| Intel UHD 6xx 9.5gen  | 16     |         |         |         |
| ARM Mali Valhall gen1 | 16     | **yes** | **yes** | no      |
| ARM Mali Midgard gen4 | no     |
| PowerVR B-Series      | 128    | no      | no      | **yes** |


## Shader instructions

| GPU | fp32 FMA/MAD | fp16x2 FMA/MAD | fp16 FMA/MAD | parallel fp32 & i32 |
|---|---|---|---|---|
| NV RTX 20xx           | yes | yes | no  | 2:1 |
| Adreno 6xx            | yes | no  | yes | ?   |
| Adreno 5xx            | yes | no  | ?   | ?   |
| AMD GCN4              | 
| Apple M1              | 
| Intel UHD 6xx 9.5gen  | yes | yes | no  | ?   | 
| ARM Mali Valhall gen1 | yes | no  | yes | 2:1 |
| ARM Mali Midgard gen4 |
| PowerVR B-Series      | yes | no  | no  | ?   |

## NaN

<details><summary><b>FP32</b></summary>

* FP32 shared

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x | nan | nan | nan | nan | inf | -inf | max | -max |
	| Min(x,0) | 0 | 0 | 0 | 0 | 0 | -inf | 0 | -max |
	| Min(0,x) | 0 | 0 | 0 | 0 | 0 | -inf | 0 | -max |
	| Max(x,0) | 0 | 0 | 0 | 0 | inf | 0 | max | 0 |
	| Max(0,x) | 0 | 0 | 0 | 0 | inf | 0 | max | 0 |
	| Clamp(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Clamp(x,-1,1) | -1 | -1 | -1 | -1 | 1 | -1 | 1 | -1 |
	| IsNaN | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |
	| IsInfinity | 0 | 0 | 0 | 0 | 1 | 1 | 0 | 0 |
	| bool(x) | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
	| Step(0,x) |  |  |  |  | 1 | 0 | 1 | 0 |
	| Step(x,0) |  |  |  |  | 0 | 1 | 0 | 1 |
	| Step(0,-x) |  |  |  |  | 0 | 1 | 0 | 1 |
	| Step(-x,0) |  |  |  |  | 1 | 0 | 1 | 0 |
	| SignOrZero(x) |  |  |  |  | 1 | -1 | 1 | -1 |
	| SignOrZero(-x) |  |  |  |  | -1 | 1 | -1 | 1 |
	| x != x |  |  |  |  | 0 | 0 | 0 | 0 |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Normalize(x) | nan | nan | nan | nan |  |  |  |  |

* FP32 on NV Turing, Adreno 5xx/6xx

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) |  |  |  |  | nan | nan | 0 | -0 |

* FP32 on Intel gen 9

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | -1 | -1 | -1 | -1 |  |  |  |  |
	| SignOrZero(-x) | -1 | -1 | -1 | -1 |  |  |  | |
	| Normalize(x) |  |  |  |  | nan | nan | 0 | -0 |

* FP32 on Mali Valhall gen1

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(-x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) |  |  |  |  | 0 | -0 | 0 | -0 |

* FP32 on Mali Midgard gen4

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) |  |  |  |  | 0 | -0 | 0 | -0 |

* FP32 on PowerVR B-Series

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | nan | nan | nan | nan |  |  |  |  |
	| SignOrZero(-x) | nan | nan | nan | nan |  |  |  |  |
	| Normalize(x) |  |  |  |  | nan | nan | 18446742974197923840 | -18446742974197923840 |

* FP32 on AMD GCN4

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Normalize(x) |  |  |  |  | 0 | 0 | 0 | 0 |

* FP32 on Apple M1

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(-x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) |  |  |  |  | nan | nan | 0 | -0 |

</details>

<details><summary><b>FP16</b></summary>

* FP16 shared

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x | nan | nan | nan | nan | inf | -inf | max | -max |
	| Min(x,0) | 0 | 0 | 0 | 0 | 0 | -inf | 0 | -max |
	| Min(0,x) | 0 | 0 | 0 | 0 | 0 | -inf | 0 | -max |
	| Max(x,0) | 0 | 0 | 0 | 0 | inf | 0 | max | 0 |
	| Max(0,x) | 0 | 0 | 0 | 0 | inf | 0 | max | 0 |
	| Clamp(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Clamp(x,-1,1) | -1 | -1 | -1 | -1 | 1 | -1 | 1 | -1 |
	| IsNaN | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |
	| IsInfinity | 0 | 0 | 0 | 0 | 1 | 1 | 0 | 0 |
	| bool(x) | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
	| x != x |  |  |  |  | 0 | 0 | 0 | 0 |
	| Step(0,x) |  |  |  |  | 1 | 0 | 1 | 0 |
	| Step(x,0) |  |  |  |  | 0 | 1 | 0 | 1 |
	| Step(0,-x) |  |  |  |  | 0 | 1 | 0 | 1 |
	| Step(-x,0) |  |  |  |  | 1 | 0 | 1 | 0 |
	| SignOrZero(x) |  |  |  |  | 1 | -1 | 1 | -1 |
	| SignOrZero(-x) |  |  |  |  | -1 | 1 | -1 | 1 |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Normalize(x) |  |  |  |  | |  | 0 | -0 |

* FP16 is not supported on Adreno 5xx, Mali Midgard gen4, AMD GCN4

* FP16 on NV Turing, Adreno 6xx

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan |  |  |
	
* FP16 on Intel gen 9

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | -1 | -1 | -1 | -1 |  |  |  |  |
	| SignOrZero(-x) | -1 | -1 | -1 | -1 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan |  |  |
	
* FP16 on Mali Valhall gen1

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(-x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) | -1 | -1 | -1 | -1 | 1 | -1 |  |  |
	
* FP16 on PowerVR B-Series

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | nan | nan | nan | nan |  |  |  |  |
	| SignOrZero(-x) | nan | nan | nan | nan |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan |  |  |
	
* FP32 on Apple M1

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x != x | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(-x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | 0 | -0 |

</details>

<details><summary><b>FP Mediump</b></summary>

* FP Mediump shared

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x | nan | nan | nan | nan | inf | -inf |  |  |
	| Min(x,0) | 0 | 0 | 0 | 0 | 0 | -inf | 0 |  |
	| Min(0,x) | 0 | 0 | 0 | 0 | 0 | -inf | 0 |  |
	| Max(x,0) | 0 | 0 | 0 | 0 | inf | 0 |  | 0 |
	| Max(0,x) | 0 | 0 | 0 | 0 | inf | 0 |  | 0 |
	| Clamp(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Clamp(x,-1,1) | -1 | -1 | -1 | -1 | 1 | -1 | 1 | -1 |
	| IsNaN | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |
	| IsInfinity | 0 | 0 | 0 | 0 | 1 | 1 | 0 | 0 |
	| bool(x) | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
	| x != x |  |  |  |  | 0 | 0 | 0 | 0 |
	| Step(0,x) |  |  |  |  | 1 | 0 | 1 | 0 |
	| Step(x,0) |  |  |  |  | 0 | 1 | 0 | 1 |
	| Step(0,-x) |  |  |  |  | 0 | 1 | 0 | 1 |
	| Step(-x,0) |  |  |  |  | 1 | 0 | 1 | 0 |
	| SignOrZero(x) |  |  |  |  | 1 | -1 | 1 | -1 |
	| SignOrZero(-x) |  |  |  |  | -1 | 1 | -1 | 1 |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 0 |
	| Normalize(x) |  |  |  |  |  |  |  |  |
	
* FP Mediump on NV Turing

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x |  |  |  |  |  |  | max | -max |
	| Min(x,0) |  |  |  |  |  |  |  | -max |
	| Min(0,x) |  |  |  |  |  |  |  | -max |
	| Max(x,0) |  |  |  |  |  |  | max |  |
	| Max(0,x) |  |  |  |  |  |  | max |  |
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | 0 | -0 |

* FP Mediump on Adreno 5xx/6xx

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x |  |  |  |  |  |  | 65504 | -65504 |
	| Min(x,0) |  |  |  |  |  |  |  | -65504 |
	| Min(0,x) |  |  |  |  |  |  |  | -65504 |
	| Max(x,0) |  |  |  |  |  |  | 65504 |  |
	| Max(0,x) |  |  |  |  |  |  | 65504 |  |
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | 255 | -255 |
	
* FP Mediump on Intel gen9

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x |  |  |  |  |  |  | max | -max |
	| Min(x,0) |  |  |  |  |  |  |  | -max |
	| Min(0,x) |  |  |  |  |  |  |  | -max |
	| Max(x,0) |  |  |  |  |  |  | max |  |
	| Max(0,x) |  |  |  |  |  |  | max |  |
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | -1 | -1 | -1 | -1 |  |  |  |  |
	| SignOrZero(-x) | -1 | -1 | -1 | -1 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | nan | nan |

* FP Mediump on Mali Valhall gen1

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x |  |  |  |  |  |  | max | -max |
	| Min(x,0) |  |  |  |  |  |  |  | -max |
	| Min(0,x) |  |  |  |  |  |  |  | -max |
	| Max(x,0) |  |  |  |  |  |  | max |  |
	| Max(0,x) |  |  |  |  |  |  | max |  |
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(-x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) | -1 | -1 | -1 | -1 | 1 | -1 | 1 | -1 |
	
* FP Mediump on Mali Midgard gen4

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x |  |  |  |  |  |  | max | -max |
	| Min(x,0) |  |  |  |  |  |  |  | -max |
	| Min(0,x) |  |  |  |  |  |  |  | -max |
	| Max(x,0) |  |  |  |  |  |  | max |  |
	| Max(0,x) |  |  |  |  |  |  | max |  |
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SmoothStep(x,0,1) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | 0 | -0 | 0 | -0 |
	
* FP Mediump on PowerVR B-Series

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x |  |  |  |  |  |  | max | -max |
	| Min(x,0) |  |  |  |  |  |  |  | -max |
	| Min(0,x) |  |  |  |  |  |  |  | -max |
	| Max(x,0) |  |  |  |  |  |  | max |  |
	| Max(0,x) |  |  |  |  |  |  | max |  |
	| x != x | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | nan | nan | nan | nan |  |  |  |  |
	| SignOrZero(-x) | nan | nan | nan | nan |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | 18446742974197923840 | -18446742974197923840 |

* FP Mediump on AMD GCN4

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x |  |  |  |  |  |  | max | -max |
	| Min(x,0) |  |  |  |  |  |  |  | -max |
	| Min(0,x) |  |  |  |  |  |  |  | -max |
	| Max(x,0) |  |  |  |  |  |  | max |  |
	| Max(0,x) |  |  |  |  |  |  | max |  |
	| x != x | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(0,-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Step(-x,0) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| SignOrZero(-x) | 1 | 1 | 1 | 1 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | 0 | 0 | 0 | 0 |
	
* FP Mediump on Apple M1

	| op \ type | nan1 | nan2 | nan3 | nan4 | inf | -inf | max | -max |
	|---|---|---|---|---|---|---|---|---|
	| x |  |  |  |  |  |  | max | -max |
	| Min(x,0) |  |  |  |  |  |  |  | -max |
	| Min(0,x) |  |  |  |  |  |  |  | -max |
	| Max(x,0) |  |  |  |  |  |  | max |  |
	| Max(0,x) |  |  |  |  |  |  | max |  |
	| x != x | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(0,-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Step(-x,0) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| SignOrZero(-x) | 0 | 0 | 0 | 0 |  |  |  |  |
	| Normalize(x) | nan | nan | nan | nan | nan | nan | 0 | -0 |

</details>

## Render target compression

**block** - compare compression between 1x1 noise and block size (4x4 or 8x8) noise.<br/>
**max** - compare compression between 1x1 noise and solid color.<br/>

| GPU | block size | block RGBA8_UNorm | max RGBA8_UNorm | block RGBA16_UNorm | max RGBA16_UNorm | method | comments |
|---|---|---|---|---|---|---|---|
| NV RTX 20xx           | 4x4   | 3   | 3.2 | 4.1 | 4.1  | exec time       |
| Adreno 6xx            | 16x16 | 1.9 | 6.9 | ?   | 3.3  | exec time       |
| Adreno 5xx            | 4x4   | 2.5 | 2.7 | ?   | ?    | exec time       |
| AMD GCN4              | 4x4   | 2.3 | 3   | 2.3 | 3    | exec time       |
| Apple M1              |
| Intel UHD 6xx 9.5gen  | 8x8   | 1.6 | 1.8 | 1.8 | 1.85 | exec time       |
| ARM Mali Valhall gen1 | 4x4   | 6.9 | 60  | -   | -    | **mem traffic** | only 32bit formats |
| PowerVR B-Series      | 8x8   | 23  | 134 | 24  | 134  | **mem traffic** |


# Test Sources

### 1. fp16 instruction performance
[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Inst-fp16.as)

### 2. fp32 instruction performance
[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Inst-fp32.as)

### 3. Render target compression
[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/RTCompression.as)

### 4. Shader instruction benchmark
[code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/graphics/Resources/pipeline/InstructionBenchmark.as)

### 5. Texture lookup performance

* sequential access - UV coordinates multiplied by scale and added bias.
	- scale < 1 has better texture cache usage.
	- scale > 1 has high cache misses.
	- scale > 1 in practice used for noise texture in procedural generation.
* 'noise NxN' - screen divided into blocks with NxN size, each block has unique offset for texture lookup, each pixel in block has 1px offset from nearest pixels.
	- offset with 1px used to find case where nearest warp can not use cached texel.
	- in practice this method is used for packed 2D sprites and textures for meshes.

[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/TexLookup.as)

### 6. Subgroups

* [Subgroups in fullscreen triangle](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Subgroups-1.as)
* [Subgroups with multiple triangles](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Subgroups-2.as)

### 7. Buffer/Image storage access

* [Image/Buffer common cases](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/Storage.as)
* [Buffer with variable data size](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/BufferStorage.as)
* [Image with thread/group reorder](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/ImageStorage-Reorder.as)
* [Image with RT compression, 4xRGBA8](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/ImageStorage-1.as)
* [Image with RT compression, 2xRGBA16](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/ImageStorage-2.as)
* [Image with RT compression, 1xR32](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/ImageStorage-3.as)

### 9. Texture cache

Find texture size where performance has near to 2x degradation this indicates a lot of cache misses and bottleneck in high level cache or external memory (RAM/VRAM).<br/>
Expected hierarchy:
* texture cache (L1)
* L2 cache
* RAM / VRAM

[code](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/perf/TexCache.as)
