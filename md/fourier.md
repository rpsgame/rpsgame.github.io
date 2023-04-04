给一个集合中的元素标号，使用下标表示，比如 $a_1$，$a_2$，$a_3$ 等，这个所有标号所形成的集合称为指标集
对于有限元素和可数无限元素的集合，其指标集是可数集，比如自然数集，如果元素个数为不可数无限，则其指标集为不可数集，比如实数

n维向量 $(x_1,x_2,\cdots,x_n)$，可以看成是由n个函数值的函数，该函数的自变量为由1到n的n个自然数，对应的函数值分别为该向量的分量值

一个函数$f(x)$，可以看成是一个无限维向量，且是不可数无限，其分量就是函数值，而自变量就是指标集，只是这个指标集是实数而非自然数

n维向量 $\mathbf{v} = (x_1,x_2,\cdots,x_n)$可以表示为标准正交基向量$\{\mathbf{e}_i\}$的线性组合
$$\displaystyle
\mathbf{v} = x_1\mathbf{e}_1 +  x_2\mathbf{e}_2 +  x_3\mathbf{e}_3 + \cdots +  x_n\mathbf{e}_n = \sum_{k=1}^{n}x_k\mathbf{e}_k
$$

而每个基向量的系数可以由内积计算得到
$$\displaystyle
x_i = \mathbf{v}\cdot\mathbf{e}_i
$$

对于函数，可以选择一组标准正交基函数$\{\mathbf{B}(x)_i\}$，同样函数$f(x)$可以表示为该基函数的线性组合
$$\displaystyle
f(x) = a_1\mathbf{B}(x)_1 + a_2\mathbf{B}(x)_2 + \cdots + a_n\mathbf{B}(x)_n + \cdots = \sum_{k=1}^{+\infty}a_k\mathbf{B}(x)_k
$$

由于函数是 不可数无限维 向量，所以这组基函数的数量为可数无限个
同样求每个基函数的系数可以由函数内积计算得到，函数内积就是两个函数的乘积在整个定义域上的积分
$$\displaystyle
a_i = \int{f(x)}\mathbf{B}(x)_i\text{d}x
$$

如下三角函数在定义域$[-\pi,\pi]$内可以构成一组正交基函数(不标准)
$$\displaystyle
1,\cos(x),\sin(x),\cos(2x),\sin(2x),\cos(3x),\sin(3x),\cdots,\cos(nx),\sin(nx),\cdots
$$

除第一个1之外，其他同一个基函数内积为 $\pi$，不同基函数两两内积为 $0$

由积化和差公式
$$\displaystyle\begin{aligned}
&\sin\alpha\cos\beta = \frac{1}{2}[\sin(\alpha+\beta)+\sin(\alpha-\beta)] \\
&\sin\alpha\sin\beta = \frac{1}{2}[\cos(\alpha+\beta)-\cos(\alpha-\beta)] \\
&\cos\alpha\cos\beta = \frac{1}{2}[\cos(\alpha+\beta)+\cos(\alpha-\beta)]
\end{aligned}$$

验证如下
当 $k \neq n$
$$\displaystyle\begin{aligned}
\int_{-\pi}^{\pi}\sin(kx)\cos(nx)\text{d}x &= \int_{-\pi}^{\pi}\frac{1}{2}[\sin(kx+nx)+\sin(kx-nx)] \\
&= -\frac{1}{2}
[\frac{\cos[(k+n)x]}{k+n}+\frac{\cos[(k-n)x]}{k-n}]_{-\pi}^{\pi} \\
&= 0
\end{aligned}$$

当 $k = n$
$$\displaystyle\begin{aligned}
\int_{-\pi}^{\pi}\sin(kx)\cos(nx)\text{d}x &= \int_{-\pi}^{\pi}\frac{1}{2}[\sin(kx+nx)+\sin(kx-nx)] \\
&= \frac{1}{2}\int_{-\pi}^{\pi}\sin(2kx)\text{d}x \\
&= -\frac{1}{2}[\frac{\cos(2kx)}{2k}]_{-\pi}^{\pi} \\
&= 0
\end{aligned}$$

函数$f(x)$表示为上述基函数线性组合的形式，即傅里叶级数
$$\displaystyle
f(x) = \frac{a_0}{2}+\sum_{k=1}^{+\infty}[a_n\cos(nx)+b_k\sin(nx)]
$$

下面求这些系数
对$f(x)$在$[-\pi,\pi]$内积分，得
$$\displaystyle
\int_{-\pi}^{\pi}f(x)\text{d}x = \frac{a_0}{2}\int_{-\pi}^{\pi}\text{d}x+\sum_{n=1}^{+\infty}[a_n\int_{-\pi}^{\pi}\cos(nx)\text{d}x+b_n\int_{-\pi}^{\pi}\sin(nx)\text{d}x]
$$

后面的求和项都是0，则
$$\displaystyle
a_0 = \frac{1}{\pi}\int_{-\pi}^{\pi}f(x)\text{d}x
$$

根据正交性，求哪个基函数的系数就与哪个基函数求内积，只有该函数与自身的内积为$\pi$，其他项均为0
$$\displaystyle
\int_{-\pi}^{\pi}f(x)\cos(nx)\text{d}x = a_n\pi
$$

得
$$\displaystyle
a_n = \frac{1}{\pi}\int_{-\pi}^{\pi}f(x)\cos(nx)\text{d}x
$$

同理可得
$$\displaystyle
b_n = \frac{1}{\pi}\int_{-\pi}^{\pi}f(x)\sin(nx)\text{d}x
$$

前面的函数周期为$2\pi$，若$f(x)$以$2l$为周期，则做变量代换$x=\frac{l}{\pi}t$，则$\varphi(t) = f(\frac{l}{\pi}t)$为周期为$2\pi$的函数，验证
$$\displaystyle
\varphi(t+2\pi) = f(\frac{l}{\pi}(t+2\pi)) = f(\frac{l}{\pi}t+2l) = f(\frac{l}{\pi}t) = \varphi(t)
$$

则函数$\varphi(t)$可以如上傅里叶级数展开为
$$\displaystyle
\varphi(t) = \frac{a_0}{2}+\sum_{n=1}^{+\infty}[a_n\cos(nt)+b_n\sin(nt)]
$$

其中
$$\displaystyle\begin{aligned}
&a_0 = \frac{1}{\pi}\int_{-\pi}^{\pi}\varphi(t)\text{d}t \\
&a_n = \frac{1}{\pi}\int_{-\pi}^{\pi}\varphi(t)\cos(nt)\text{d}t \\
&b_n = \frac{1}{\pi}\int_{-\pi}^{\pi}\varphi(t)\sin(nt)\text{d}t
\end{aligned}$$

再通过带入反变量代换$t=\frac{\pi}{l}x$，并令$\omega_n=\frac{n\pi}{l}$，得
$$\displaystyle
f(x) = \frac{a_0}{2}+\sum_{n=1}^{+\infty}[a_n\cos(\omega_nx)+b_n\sin(\omega_nx)]
$$

其中
$$\displaystyle\begin{aligned}
&a_0 = \frac{1}{l}\int_{-l}^{l}f(x)\text{d}x \\
&a_n = \frac{1}{l}\int_{-l}^{l}f(x)\cos(\omega_nx)\text{d}x \\
&b_n = \frac{1}{l}\int_{-l}^{l}f(x)\sin(\omega_nx)\text{d}x
\end{aligned}$$

由欧拉公式 $e^{j\varphi} = \cos\varphi+j\sin\varphi$，得
$$\displaystyle\begin{aligned}
&\cos(\omega_nx) = \frac{1}{2}e^{j\omega_nx}+\frac{1}{2}e^{-j\omega_nx} \\
&\sin(\omega_nx) = -j(\frac{1}{2}e^{j\omega_nx}-\frac{1}{2}e^{-j\omega_nx}) \\
\end{aligned}$$

带入$f(x)$的傅里叶展开式，得其复数形式
$$\displaystyle
f(x) = c_0+\sum_{n=1}^{+\infty}[c_ne^{j\omega_nx}+c_{-n}e^{-j\omega_nx}]
$$

其中
$$\displaystyle\begin{aligned}
&c_0 = \frac{a_0}{2} = \frac{1}{2l}\int_{-l}^{l}f(\tau)\text{d}\tau \\
&c_n = \frac{a_n-jb_n}{2} = \frac{1}{2l}\int_{-l}^{l}f(\tau)e^{-j\omega_n\tau}\text{d}\tau \\
&c_{-n} = \frac{a_n+jb_n}{2} = \frac{1}{2l}\int_{-l}^{l}f(\tau)e^{j\omega_n\tau}\text{d}\tau
\end{aligned}$$

写成统一形式
$$\displaystyle
f(x) = \sum_{n=-\infty}^{+\infty}c_ne^{j\omega_nx}
$$

带入$c_n$，并令$\Delta\omega_n = \omega_n-\omega_{n-1} = \frac{n\pi}{l}-\frac{(n-1)\pi}{l} = \frac{\pi}{l}$，得
$$\displaystyle\begin{aligned}
f(x) &= \sum_{n=-\infty}^{+\infty}c_ne^{j\omega_nx} \\
&= \sum_{n=-\infty}^{+\infty}[\frac{1}{2l}\int_{-l}^{l}f(\tau)e^{-j\omega_n\tau}\text{d}\tau]e^{j\omega_nx} \\
&= \frac{1}{2\pi}\sum_{n=-\infty}^{+\infty}[\int_{-l}^{l}f(\tau)e^{-j\omega_n\tau}\text{d}\tau]e^{j\omega_nx}\Delta\omega_n
\end{aligned}$$

每个$\omega_n$都对应一个n值，也对应一个基函数，这是一个可数无限的指标集，如果指标集为不可数无限，则对应不可数无限个基函数，此时有$\Delta\omega_n\to0$时，上面的求和符号是由可数无限的基函数线性组合而来，此时改为不可数无限个基函数，则求和符号改为积分符号，并令周期$l\to+\infty$，得到
$$\displaystyle
f(x) = \frac{1}{2\pi}\int_{n=-\infty}^{+\infty}[\int_{-\infty}^{+\infty}f(\tau)e^{-j\omega\tau}\text{d}\tau]e^{j\omega x}\text{d}\omega
$$

此为傅里叶积分公式，内部积分是在求基函数系数，外部积分是在求基函数的线性组合

令
$$\displaystyle
F(\omega) = \int_{-\infty}^{+\infty}f(\tau)e^{-j\omega\tau}\text{d}\tau
$$

则
$$\displaystyle
f(x) = \frac{1}{2\pi}\int_{n=-\infty}^{+\infty}F(\omega)e^{j\omega x}\text{d}\omega
$$

前者为傅里叶变换，后者为傅里叶逆变换，分别记为
$$\displaystyle\begin{aligned}
&F(\omega) = \mathscr{F}[f(x)] \\
&f(x) = \mathscr{F}^{-1}[F(\omega)]
\end{aligned}$$


对连续函数$f(\tau)$取N个离散采样点，$f_k = f(\tau_k)$，$\tau_k = k\Delta\tau$，$k = 0,1,2,\cdots,N-1$，其中 $\Delta\tau$ 为采样间隔

对连续函数$f(\tau)$进行傅里叶变换后的函数$F(\omega)$，也只取其中N个值，$F_n=F(\omega_n)$，取 $\omega_n=\frac{2\pi}{N\Delta\tau}n$，$n\in[-\frac{N}{2},\frac{N}{2}]$，$N\Delta\tau$ 是$f(\tau)$所有采样点的采样总区间范围

则连续傅里叶变换可近似为离散傅里叶变换
$$\displaystyle\begin{aligned}
F_n = F(\omega_n) &= \int_{-\infty}^{+\infty}f(\tau)e^{-j\omega_n\tau}\text{d}\tau \\
&\approx \sum_{k=0}^{N-1}f(\tau_k)e^{-j\omega_n\tau_k}\Delta\tau \\
&\approx \Delta\tau\sum_{k=0}^{N-1}f_ke^{-j\frac{2\pi}{N\Delta\tau}n k\Delta\tau} \\
&\approx \Delta\tau\sum_{k=0}^{N-1}f_ke^{-j\frac{2\pi n}{N}k}
\end{aligned}$$

取后面的和式为离散傅里叶变换
$$\displaystyle
F_n = \sum_{k=0}^{N-1}f_ke^{-j\frac{2\pi n}{N}k}
$$
