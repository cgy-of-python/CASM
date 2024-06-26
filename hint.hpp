#include <iostream>
#include <future>
#include <thread>
#include <atomic>
#include <algorithm>
#include <random>
#include <stack>
#include <string>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <climits>

#ifndef HINT_HPP
#define HINT_HPP

// 取消对宏MULTITHREAD的注释即可开启多线程
// #define MULTITHREAD

namespace hint {
    using UINT_8 = uint8_t;
    using UINT_16 = uint16_t;
    using UINT_32 = uint32_t;
    using UINT_64 = uint64_t;
    using INT_32 = int32_t;
    using INT_64 = int64_t;
    using ULONG = unsigned long;
    using LONG = long;

    constexpr UINT_64 HINT_CHAR_BIT = 8;
    constexpr UINT_64 HINT_SHORT_BIT = 16;
    constexpr UINT_64 HINT_INT_BIT = 32;
    constexpr UINT_64 HINT_INT8_0XFF = UINT8_MAX;
    constexpr UINT_64 HINT_INT8_0X10 = (UINT8_MAX + 1ull);
    constexpr UINT_64 HINT_INT16_0XFF = UINT16_MAX;
    constexpr UINT_64 HINT_INT16_0X10 = (UINT16_MAX + 1ull);
    constexpr UINT_64 HINT_INT32_0XFF = UINT32_MAX;
    constexpr UINT_64 HINT_INT32_0X01 = 1;
    constexpr UINT_64 HINT_INT32_0X80 = 0X80000000ull;
    constexpr UINT_64 HINT_INT32_0X7F = INT32_MAX;
    constexpr UINT_64 HINT_INT32_0X10 = (UINT32_MAX + 1ull);
    constexpr UINT_64 HINT_INT64_0X80 = INT64_MIN;
    constexpr UINT_64 HINT_INT64_0X7F = INT64_MAX;

#if SIZE_MAX == 18446744073709551615ull
    using hint_size_t = int64_t;
    constexpr UINT_64 HINT_SIZE_0X80 = HINT_INT64_0X80;
    constexpr UINT_64 HINT_SIZE_0X7F = HINT_INT64_0X7F;
#elif SIZE_MAX == 4294967295
    using hint_size_t = int32_t;
    constexpr UINT_64 HINT_SIZE_0X80 = HINT_INT32_0X80;
    constexpr UINT_64 HINT_SIZE_0X7F = HINT_INT32_0X7F;
#else
#error "Unknown sys bits"
#endif
    constexpr size_t HINT_FFT_MIN = 128ull;
    constexpr size_t HINT_FFT_NLUT_MAX = 4096ull;  //不查表的最大长度
    constexpr size_t HINT_FFT_LUT_MAX = 131072ull; // 2^17为2分fft结果最大长度
    constexpr size_t HINT_NTT_MAX = 67108864ull;   // 2^26为2分ntt结果最大长度
    constexpr size_t HINT_NTT_MULTHLEN = 1024ull;  //设置触发多线程的ntt长度
    constexpr double HINT_PI = 3.1415926535897932384626433832795;
    constexpr double HINT_2PI = 6.283185307179586476925286766559;

    struct h_int {
        hint::UINT_32 *array = nullptr;
        hint::INT_64 neg_n_len = 0;
        size_t size = 0;
    };

    struct Complex //对复数的定义
    {
        double real = 0, imaginary = 0;

        Complex() {
            real = imaginary = 0;
        }

        Complex(const Complex &input) {
            real = input.real;
            imaginary = input.imaginary;
        }

        Complex(double r, double i) {
            real = r;
            imaginary = i;
        }

        Complex(hint::UINT_64 n) // n等分圆周的复数,即x^n=1的解中除x=1以外辐角最小的一个解
        {
            real = std::cos(HINT_2PI / n);
            imaginary = std::sin(HINT_2PI / n);
        }

        Complex operator=(Complex input) {
            real = input.real;
            imaginary = input.imaginary;
            return *this;
        }

        Complex operator+(Complex input) const //复数加法
        {
            return Complex(real + input.real, imaginary + input.imaginary);
        }

        Complex operator-(Complex input) const //复数减法
        {
            return Complex(real - input.real, imaginary - input.imaginary);
        }

        Complex operator*(Complex input) const //复数乘法
        {
            return Complex(real * input.real - imaginary * input.imaginary,
                           real * input.imaginary + imaginary * input.real);
        }

        Complex &operator*=(Complex input) //复数乘法
        {
            double tmp_real = real * input.real - imaginary * input.imaginary;
            imaginary = real * input.imaginary + imaginary * input.real;
            real = tmp_real;
            return *this;
        }

        Complex operator/(Complex input) const //复数除法
        {
            double tmp = input.real * input.real + input.imaginary * input.imaginary;
            double re = real * input.real + imaginary * input.imaginary;
            double img = imaginary * input.real - real * input.imaginary;
            return Complex(re / tmp, img / tmp);
        }

        void self_add(Complex input) {
            real += input.real;
            imaginary += input.imaginary;
        }

        void self_mul(Complex input) {
            double tmp_real = real * input.real - imaginary * input.imaginary;
            imaginary = real * input.imaginary + imaginary * input.real;
            real = tmp_real;
        }

        void console_out() const //打印复数
        {
            std::cout << real;
            if (imaginary < 0)
                std::cout << imaginary << "i";
            else
                std::cout << "+" << imaginary << "i";
        }
    };

    const UINT_32 hint_threads = std::thread::hardware_concurrency();
    const UINT_32 log2_threads = std::ceil(std::log2(hint_threads));
    std::atomic<UINT_32> cur_ths;

    template<typename T>
    constexpr bool is_neg(T x) {
        return x < 0;
    }

    template<typename T>
    constexpr bool is_odd(T x) {
        return static_cast<bool>(x & 1);
    }

    template<typename T>
    constexpr T twice(T x) {
        return x * 2;
    }

    template<typename T>
    constexpr T half(T x) {
        return x / 2;
    }

    template<typename T>
    constexpr void self_twice(T &x) {
        x *= 2;
    }

    template<typename T>
    constexpr void self_half(T &x) {
        x /= 2;
    }

    template<typename T>
    constexpr T qpow(T m, hint::UINT_64 n) //模板快速幂
    {
        T result = 1;
        while (n > 0) {
            if ((n & 1) != 0) {
                result = result * m;
            }
            m = m * m;
            n >>= 1;
        }
        return result;
    }

    constexpr UINT_64 qpow(UINT_64 m, UINT_64 n, UINT_64 mod) //取模快速幂
    {
        UINT_64 result = 1;
        while (n > 0) {
            if ((n & 1) != 0) {
                result = result * m % mod;
            }
            m = m * m % mod;
            n >>= 1;
        }
        return result;
    }

    constexpr UINT_64 gcd(UINT_64 a, UINT_64 b) //最大公因数
    {
        if (b == 0) {
            return a;
        }
        UINT_64 tmp = b;
        b = a % b;
        a = tmp;
        while (b > 0) {
            tmp = b;
            b = a % b;
            a = tmp;
        }
        return a;
    }

    UINT_64 crt(UINT_64 *mods, UINT_64 *nums, size_t n) //中国剩余定理
    {
        UINT_64 result = 0, mod_product = 1;
        for (size_t i = 0; i < n; i++) {
            mod_product *= mods[i];
        }
        for (size_t i = 0; i < n; i++) {
            UINT_64 mod = mods[i];
            UINT_64 tmp = mod_product / mod;
            UINT_64 inv = qpow(tmp, mod - 2, mod);
            result += nums[i] * tmp * inv % mod_product;
        }
        return result % mod_product;
    }

    constexpr UINT_64 qcrt(UINT_64 num1, UINT_64 num2,
                           UINT_64 mod1 = 167772161, UINT_64 mod2 = 469762049,
                           UINT_64 inv1 = 104391568, UINT_64 inv2 = 130489458) //快速计算两模数的中国剩余定理
    {
        if (num1 > num2) {
            return ((num1 - num2) * inv2 % mod1) * mod2 + num2;
        } else {
            return ((num2 - num1) * inv1 % mod2) * mod1 + num1;
        }
    }

    template<typename T>
    inline T *ary_copy(T *target, const T *source, size_t len) //模板数组拷贝
    {
        return static_cast<T *>(std::memcpy(target, source, len * sizeof(T)));
    }

    template<typename T>
    inline void com_ary_real_copy(Complex *target, const T *source, size_t len) //从其他类型数组拷贝到复数组
    {
        for (size_t i = 0; i < len; i++) {
            target[i].real = source[i];
        }
    }

    template<typename T>
    inline void com_ary_img_copy(Complex *target, const T *source, size_t len) //从其他类型数组拷贝到复数组
    {
        for (size_t i = 0; i < len; i++) {
            target[i].imaginary = source[i];
        }
    }

    template<typename T>
    inline void ary_calloc(T *&ptr, size_t len) //模版数组分配内存且清零
    {
        ptr = static_cast<T *>(calloc(len, sizeof(T)));
    }

    template<typename T>
    inline void ary_clr(T *ptr, size_t len) //模版数组清零
    {
        memset(ptr, 0, len * sizeof(T));
    }

    template<typename T>
    inline T *ary_realloc(T *ptr, size_t len) //重分配空间
    {
        if (len * sizeof(T) < INT64_MAX) {
            return static_cast<T *>(realloc(ptr, len * sizeof(T)));
        }
        throw ("realloc error");
        return nullptr;
    }

    constexpr size_t max_len = 1ull << 20;
    Complex unit[max_len];

    inline Complex *unit_ary_init(Complex unit_ary[]) {
        unit_ary[0].real = unit_ary[max_len / 2].real = 1;
        for (size_t i = 1; i < max_len / 2; i++) {
            Complex tmp(std::cos(i * HINT_2PI / max_len), std::sin(i * HINT_2PI / max_len));
            unit_ary[i + max_len / 2].real = unit_ary[i].real = tmp.real;
            unit_ary[i].imaginary = tmp.imaginary;
            unit_ary[i + max_len / 2].imaginary = -tmp.imaginary;
        }
        return unit_ary;
    }

    const Complex *unit_ptr = unit_ary_init(unit);

    /// @brief 快速傅里叶变换，对长度为fft_len的复数组进行快速傅里叶变换
    /// @param input
    /// @param fft_len
    /// @param is_ifft
    /// @param lut
    void fft(Complex *input, size_t fft_len, const bool is_ifft, const bool lut) //快速傅里叶(逆)变换
    {
        size_t log_n = static_cast<size_t>(log2(fft_len));
        fft_len = 1ull << log_n;
        if (lut && fft_len > max_len) {
            throw ("FFT len too long");
        }
        size_t *rev = new size_t[fft_len];
        rev[0] = 0;
        for (size_t i = 1; i < fft_len; i++) {
            rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (log_n - 1)); //求rev交换数组
        }
        for (size_t i = 0; i < fft_len; i++) {
            size_t index = rev[i];
            if (i < index) {
                Complex tmp = input[i];
                input[i] = input[index];
                input[index] = tmp;
            }
        }
        delete[] rev;
        if (lut) {
            Complex tmp1, tmp2;
            UINT_16 shift = 0;
            const size_t offset = is_ifft ? half(max_len) : 0;
            for (size_t rank = 1, gap; rank < fft_len; self_twice(rank)) {
                gap = twice(rank);
                shift++;
                for (size_t begin = 0; begin < fft_len; begin += gap) {
                    for (size_t pos = begin; pos < begin + rank; pos++) {
                        tmp1 = input[pos];
                        tmp2 = input[pos + rank] * unit_ptr[offset + (pos - begin) * (max_len >> shift)];
                        input[pos].self_add(tmp2);
                        input[pos + rank] = tmp1 - tmp2;
                    }
                }
            }
        } else {
            Complex unit_omega, omega, tmp1, tmp2;
            for (size_t rank = 1, gap; rank < fft_len; self_twice(rank)) {
                gap = twice(rank);
                unit_omega = Complex(gap);
                if (is_ifft) {
                    unit_omega.imaginary = -unit_omega.imaginary;
                }
                for (size_t begin = 0; begin < fft_len; begin += gap) {
                    omega = Complex(1, 0);
                    for (size_t pos = begin; pos < begin + rank; pos++) {
                        tmp1 = input[pos];
                        tmp2 = input[pos + rank] * omega;
                        input[pos].self_add(tmp2);
                        input[pos + rank] = tmp1 - tmp2;
                        omega.self_mul(unit_omega);
                        if (rank == (pos - begin + 1) << 1) {
                            omega = is_ifft ? Complex(0, -1) : Complex(0, 1);
                        }
                    }
                }
            }
        }
        if (is_ifft) //逆变换需除以n
        {
            double len = static_cast<double>(fft_len);
            for (size_t i = 0; i < fft_len; i++) {
                input[i].real /= len;
                input[i].imaginary /= len;
            }
        }
    }

    /// @brief 快速数论变换,对长度为ntt_len的整数数组进行快速数论变换
    /// @param input
    /// @param ntt_len
    /// @param is_intt
    /// @param mod
    /// @param g_root
    void ntt(UINT_64 *input, size_t ntt_len, bool is_intt, const UINT_64 mod = 998244353, UINT_64 g_root = 3) //快速数论变换
    {
        size_t log_n = static_cast<size_t>(log2(ntt_len));
        size_t *rev = new size_t[ntt_len];
        ntt_len = 1ull << log_n;
        rev[0] = 0;
        for (size_t i = 1; i < ntt_len; i++) {
            rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (log_n - 1));
        }
        for (size_t i = 0; i < ntt_len; i++) {
            size_t index = rev[i];
            if (i < index) {
                UINT_64 tmp = input[i];
                input[i] = input[index];
                input[index] = tmp;
            }
        }
        delete[] rev;
        if (is_intt) {
            g_root = qpow(g_root, mod - 2, mod);
        }
        UINT_64 unit_omega, omega;
        for (size_t rank = 1, gap; rank < ntt_len; self_twice(rank)) {
            gap = twice(rank);
            unit_omega = qpow(g_root, (mod - 1) / gap, mod);
            for (size_t begin = 0; begin < ntt_len; begin += gap) {
                omega = 1;
                for (size_t pos = begin; pos < begin + rank; pos++) {
                    UINT_64 tmp1 = input[pos];
                    UINT_64 tmp2 = (input[pos + rank] % mod) * omega % mod;
                    input[pos] = (tmp1 + tmp2) % mod;
                    input[pos + rank] = (mod + tmp1 - tmp2) % mod;
                    omega = omega * unit_omega % mod;
                }
            }
        }
        if (is_intt) {
            UINT_64 inv = qpow(ntt_len, mod - 2, mod);
            for (size_t i = 0; i < ntt_len; ++i) {
                input[i] = input[i] * inv % mod;
            }
        }
    }

    template<typename T1, typename T2, typename T3>
    void normal_convolution(T1 *const ary1, T2 *const ary2, T3 *const out, size_t len1, size_t len2) {
        ary_clr(out, len1 + len2);
        for (size_t i = 0; i < len1; i++) {
            for (size_t j = 0; j < len2; j++) {
                out[i + j] += static_cast<T2>(ary1[i]) * ary2[j];
            }
        }
    }

    inline void fft_convolution(Complex *const fft_ary1, Complex *const fft_ary2, Complex *const out, size_t fft_len,
                                const bool lut = false) //快速傅里叶变换卷积分
    {
#ifdef MULTITHREAD
        bool multi_threads = hint_threads >= 2 && fft_len >= 2 * HINT_FFT_NLUT_MAX;
        if (multi_threads)
        {
            std::future<void> fft_th = std::async(fft, fft_ary1, fft_len, false, lut); //快速傅里叶变换
            if (fft_ary1 != fft_ary2)
            {
                fft(fft_ary2, fft_len, false, lut);
            }
            fft_th.wait();
        }
        else
#endif
        {
            fft(fft_ary1, fft_len, false, lut); //快速傅里叶变换
            if (fft_ary1 != fft_ary2) {
                fft(fft_ary2, fft_len, false, lut);
            }
            //每一位相乘
        }
        for (size_t i = 0; i < fft_len; i++) {
            out[i] = fft_ary1[i] * fft_ary2[i];
        }
        fft(out, fft_len, true, lut); //逆变换
    }

    void ntt_convolution(UINT_64 *const ntt_ary1, UINT_64 *const ntt_ary2, UINT_64 *const out, size_t ntt_len) //数论变换卷积分
    {
        constexpr UINT_64 mod1 = 2013265921, mod2 = 2281701377;
        constexpr UINT_64 root1 = 31, root2 = 3;
        UINT_64 *ntt_ary3 = nullptr, *ntt_ary4 = nullptr;
        if (ntt_ary1 == ntt_ary2) {
            ntt_ary3 = ntt_ary4 = new UINT_64[ntt_len];
            ary_copy(ntt_ary3, ntt_ary1, ntt_len);
        } else {
            ntt_ary3 = new UINT_64[ntt_len * 2];
            ntt_ary4 = ntt_ary3 + ntt_len;
            ary_copy(ntt_ary3, ntt_ary1, ntt_len);
            ary_copy(ntt_ary4, ntt_ary2, ntt_len);
        }
        std::function<void(UINT_64 *, UINT_64 *)> mul_func = [=](UINT_64 *ary1, UINT_64 *ary2) {
            for (size_t i = 0; i < ntt_len; i++) {
                ary1[i] = ary1[i] * ary2[i];
            } //每一位相乘
        };
#ifdef MULTITHREAD
        bool multi_threads = hint_threads >= 2 && ntt_len > HINT_NTT_MULTHLEN;
        if (multi_threads)
        {
            std::future<void> ntt_th1 = std::async(ntt, ntt_ary1, ntt_len, false, mod1, root1); // 多线程快速数论变换
            std::future<void> ntt_th2 = std::async(ntt, ntt_ary3, ntt_len, false, mod2, root2);
            if (ntt_ary1 != ntt_ary2)
            {
                std::future<void> ntt_th3 = std::async(ntt, ntt_ary2, ntt_len, false, mod1, root1);
                std::future<void> ntt_th4 = std::async(ntt, ntt_ary4, ntt_len, false, mod2, root2);
                ntt_th3.wait();
                ntt_th4.wait();
            }
            ntt_th1.wait();
            ntt_th2.wait();

            mul_func(ntt_ary1, ntt_ary2);
            mul_func(ntt_ary3, ntt_ary4); //每一位相乘

            std::future<void> intt_th = std::async(ntt, ntt_ary1, ntt_len, true, mod1, root1);
            ntt(ntt_ary3, ntt_len, true, mod2, root2);
            intt_th.wait();
        }
        else
#endif
        {
            ntt(ntt_ary1, ntt_len, false, mod1, root1); //快速数论变换
            ntt(ntt_ary3, ntt_len, false, mod2, root2);
            if (ntt_ary1 != ntt_ary2) {
                ntt(ntt_ary2, ntt_len, false, mod1, root1); //快速数论变换
                ntt(ntt_ary4, ntt_len, false, mod2, root2);
            }

            mul_func(ntt_ary1, ntt_ary2);
            mul_func(ntt_ary3, ntt_ary4); //每一位相乘

            ntt(ntt_ary1, ntt_len, true, mod1, root1); //逆变换
            ntt(ntt_ary3, ntt_len, true, mod2, root2);
        }
        constexpr UINT_64 inv1 = qpow(mod1, mod2 - 2, mod2);
        constexpr UINT_64 inv2 = qpow(mod2, mod1 - 2, mod1);
        for (size_t i = 0; i < ntt_len; i++) {
            out[i] = qcrt(ntt_ary1[i], ntt_ary3[i], mod1, mod2, inv1, inv2);
        } //使用中国剩余定理变换
        delete[] ntt_ary3;
    }

    template<typename T1, typename T2, typename T3>
    void trans_add(const T1 *in1, const T2 *in2, T3 *out, size_t len1, size_t len2,
                   const INT_64 base = 100) //可计算多项式的加法,默认为100进制
    {
        size_t result_len = std::max(len1, len2);
        INT_64 tmp = 0;
        size_t count = 0;
        while (count < result_len) {
            if (count < len1) {
                tmp += in1[count];
            }
            if (count < len2) {
                tmp += in2[count];
            }
            out[count] = static_cast<T3>(tmp % base);
            tmp /= base;
            count++;
        }
        if (tmp > 0) {
            out[count] = static_cast<T3>(tmp % base);
        }
    }

    template<typename T1, typename T2, typename T3>
    void trans_mul(const T1 *in1, const T2 *in2, T3 *out, size_t len1, size_t len2, const INT_64 base = 100) //计算多项式的乘法
    {
        size_t out_len = len1 + len2;
        while (len1 > 0 && in1[len1 - 1] == 0) {
            len1--;
        }
        while (len2 > 0 && in2[len2 - 1] == 0) {
            len2--;
        }
        if (out_len < 640) {
            UINT_64 *con_result = new UINT_64[out_len];
            ary_clr(con_result, out_len);
            normal_convolution(in1, in2, con_result, len1, len2);
            UINT_64 tmp = 0;
            size_t pos = 0;
            while (pos < out_len) {
                tmp += con_result[pos];
                out[pos] = static_cast<T3>(tmp % base);
                tmp /= base;
                pos++;
            } //整理每一位
            delete[] con_result;
        } else if (out_len <= 2097152) {
            size_t fft_len = 1ull << static_cast<UINT_16>(ceil(log2(out_len)));
            Complex *fft_in1 = new Complex[fft_len];
            // Complex *fft_in2 = fft_in1 + fft_len;
            com_ary_real_copy(fft_in1, in1, len1);
            com_ary_img_copy(fft_in1, in2, len2);
            fft_convolution(fft_in1, fft_in1, fft_in1, fft_len);
            UINT_64 tmp = 0;
            size_t pos = 0;
            while (pos < out_len) {
                tmp += static_cast<UINT_64>(fft_in1[pos].imaginary / 2 + 0.5);
                out[pos] = static_cast<T3>(tmp % base);
                tmp /= base;
                pos++;
            } //整理每一位
            delete[] fft_in1;
        } else if (out_len <= 1073741824) {
            size_t ntt_len = 1ull << static_cast<UINT_16>(ceil(log2(out_len)));
            UINT_64 *ntt_ary1 = new UINT_64[ntt_len * 2];
            UINT_64 *ntt_ary2 = ntt_ary1 + ntt_len;
            for (size_t i = 0; i < len1; i++) {
                ntt_ary1[i] = static_cast<UINT_64>(in1[i]);
            }
            for (size_t i = 0; i < len2; i++) {
                ntt_ary2[i] = static_cast<UINT_64>(in2[i]);
            }
            ary_clr(ntt_ary1 + len1, ntt_len - len1);
            ary_clr(ntt_ary2 + len2, ntt_len - len2);
            ntt_convolution(ntt_ary1, ntt_ary2, ntt_ary1, ntt_len);
            UINT_64 tmp = 0;
            size_t pos = 0;
            while (pos < out_len) {
                tmp += static_cast<UINT_64>(ntt_ary1[pos]);
                out[pos] = static_cast<T2>(tmp % base);
                tmp /= base;
                pos++;
            } //整理每一位
            delete[] ntt_ary1;
        } else {
            throw ("Mul length error: too long");
        }
    }

    template<typename T1, typename T2>
    void trans_square(const T1 *in, T2 *out, size_t len, const INT_64 base = 100) //平方
    {
        while (len > 0 && in[len - 1] == 0) {
            len--;
        }
        size_t out_len = twice(len);
        if (out_len < 640) {
            UINT_64 *con_result = new UINT_64[out_len];
            ary_clr(con_result, out_len);
            normal_convolution(in, in, con_result, len, len);
            UINT_64 tmp = 0;
            size_t pos = 0;
            while (pos < out_len) {
                tmp += con_result[pos];
                out[pos] = static_cast<T2>(tmp % base);
                tmp /= base;
                pos++;
            } //整理每一位
            delete[] con_result;
        } else if (out_len <= 2097152) {
            size_t fft_len = 1ull << static_cast<UINT_16>(ceil(log2(out_len)));
            Complex *fft_ary = new Complex[fft_len];
            com_ary_real_copy(fft_ary, in, len);
            fft_convolution(fft_ary, fft_ary, fft_ary, fft_len);
            UINT_64 tmp = 0;
            size_t pos = 0;
            while (pos < out_len) {
                tmp += static_cast<UINT_64>(fft_ary[pos].real + 0.5);
                out[pos] = static_cast<T2>(tmp % base);
                tmp /= base;
                pos++;
            } //整理每一位
            delete[] fft_ary;
        } else if (out_len <= 1073741824) {
            size_t ntt_len = 1ull << static_cast<UINT_16>(ceil(log2(out_len)));
            UINT_64 *ntt_ary = new UINT_64[ntt_len];
            for (size_t i = 0; i < len; i++) {
                ntt_ary[i] = static_cast<UINT_64>(in[i]);
            }
            ary_clr(ntt_ary + len, ntt_len - len);
            ntt_convolution(ntt_ary, ntt_ary, ntt_ary, ntt_len);
            UINT_64 tmp = 0;
            size_t pos = 0;
            while (pos < out_len) {
                tmp += static_cast<UINT_64>(ntt_ary[pos]);
                out[pos] = static_cast<T2>(tmp % base);
                tmp /= base;
                pos++;
            } //整理每一位
            delete[] ntt_ary;
        } else {
            throw ("Sq length error: too long");
        }
    }

    template<typename T, typename UNIT_T = UINT_8>
    void
    base_conversion(T *data_ary, size_t &in_len, const UINT_32 BASE1, const UINT_32 BASE2) // 256进制转为100进制的数组方便打印和转字符串
    {
        if (in_len == 0 || BASE1 == BASE2) {
            return;
        }
        if (in_len <= 1) {
            UINT_64 tmp = data_ary[0];
            data_ary[0] = static_cast<T>(tmp % BASE2);
            tmp /= BASE2;
            size_t pos = 1;
            while (tmp > 0) {
                data_ary[pos] = static_cast<T>(tmp);
                in_len++;
                pos++;
            }
            return;
        }
        size_t max_rank = 1ull
                << static_cast<UINT_16>(ceil(log2(in_len)) - 1);                            // unit_ary存储的base1的最高次幂
        const UINT_64 base1to2_len = static_cast<UINT_64>(std::ceil(
                std::log2(BASE1) / std::log2(BASE2))); // base1到base2的数长度的比值
        size_t unit_ary_num =
                static_cast<UINT_16>(log2(max_rank)) + 1;                                    // unit_ary存储的base1各个幂次的个数
        size_t result_len = static_cast<size_t>(base1to2_len * twice(max_rank));                           // 结果的长度
        ary_clr(data_ary + in_len, result_len - in_len);                                                   // 清零

        size_t unit_ary_len = (twice(max_rank) - 1) * base1to2_len; // unit_ary的长度1+2+4+...max_rank
        UNIT_T *unit_ary = new UNIT_T[unit_ary_len];                // 用一个数组存储(base1)^1,(base1)^2,(base1)^4...
        ary_clr(unit_ary, unit_ary_len);
        unit_ary[0] = BASE1 % BASE2;
        if (base1to2_len > 1) {
            size_t pos = in_len;
            while (pos > 0) {
                pos--;
                UINT_64 tmp = data_ary[pos];
                size_t trans_pos = pos * base1to2_len;
                for (size_t i = 0; i < base1to2_len; i++) {
                    data_ary[trans_pos + i] = static_cast<T>(tmp % BASE2);
                    tmp /= BASE2;
                }
            }
            pos = 1;
            UINT_64 tmp = BASE1 / BASE2;
            while (tmp > 0) {
                unit_ary[pos] = static_cast<hint::UINT_8>(tmp % BASE2);
                tmp /= BASE2;
                pos++;
            }
            UNIT_T test = BASE1 - 1;
            bool ret = true;
            while (test > 0) {
                ret = ret && ((test % BASE2) == (BASE2 - 1));
                test /= BASE2;
            }
            if (ret) {
                in_len = result_len;
                return;
            }
        }
        for (size_t i = 0, offset = 0; i < unit_ary_num - 1; i++) {
            size_t len = (1ull << i) * base1to2_len;
            trans_square(unit_ary + offset, unit_ary + offset + len, len, BASE2);
            offset += len;
        }
        UINT_8 *tmp_product = new UINT_8[base1to2_len * 2];
        for (size_t i = 0, offset = 0; i < unit_ary_num; i++) {
            size_t len = (1ull << i) * base1to2_len;
            size_t gap = twice(len);
            size_t pos = 0;
            tmp_product = ary_realloc(tmp_product, gap);
            // ary_clr(tmp_product, gap);
            while (pos < result_len) {
                trans_mul(unit_ary + offset, data_ary + pos + len, tmp_product, len, len, BASE2);
                trans_add(tmp_product, data_ary + pos, data_ary + pos, gap, len, BASE2);
                // ary_copy(data_ary + pos, tmp_product, gap);
                pos += gap;
            }
            offset += len;
        }
        while (data_ary[result_len - 1] == 0) {
            result_len--;
        }
        in_len = result_len;
        delete[] tmp_product;
    }

    std::string ui64to_string(UINT_64 input, UINT_8 digits) {
        std::string result(digits, '0');
        for (UINT_8 i = 0; i < digits; i++) {
            result[digits - i - 1] = static_cast<char>(input % 10 + '0');
            input /= 10;
        }
        return result;
    }

    UINT_64 stoui64(const std::string &str, const UINT_32 base = 10) {
        UINT_64 result = 0;
        size_t len = str.size();
        for (size_t i = 0; i < len && i < 19; i++) {
            result *= base;
            char c = tolower(str[i]);
            UINT_64 n = static_cast<UINT_64>(c);
            if (isalnum(c)) {
                if (isalpha(c)) {
                    n -= 'a';
                    n += 10;
                } else {
                    n -= '0';
                }
            } else {
                n = 0;
            }
            if (n < base) {
                result += n;
            }
        }
        return result;
    }
}
class HyperInt {
private:
    hint::h_int data;

    constexpr void change_length(size_t new_length) //设置新的长度
    {
        if (new_length > data.size) {
            new_length = data.size;
        }
        if (new_length == 0) {
            neg_sign(false);
        }
        data.neg_n_len = (data.neg_n_len & hint::HINT_SIZE_0X80) | new_length;
    }

    constexpr size_t generate_size(size_t new_size_input) const //生成1.5倍数组空间
    {
        if (new_size_input >= hint::HINT_SIZE_0X7F) {
            return hint::HINT_SIZE_0X7F;
        }
        if (new_size_input <= 2) {
            return 2;
        }
        size_t size1 = 1ull << static_cast<hint::UINT_16>(ceil(log2(new_size_input)));
        size_t size2 = hint::half(size1);
        size2 = size2 + hint::half(size2);
        if (new_size_input <= size2) {
            return size2;
        } else {
            return size1;
        }
    }

    //清空
    void clear() {
        neg_sign(false);
        hint::ary_clr(data.array, data.size);
    }

    //填充相同的元素
    void fill_element(hint::UINT_32 element) {
        std::fill(data.array, data.array + length(), element);
    }

    //快速左移一位,只能操作大小为2^n的数
    void quick_self_twice() {
        size_t len_tmp = length();
        if (len_tmp > 0) {
            hint::UINT_32 tmp = data.array[len_tmp - 1];
            hint::self_twice(tmp);
            if (tmp == 0) {
                len_tmp++;
                if (len_tmp > data.size) {
                    return;
                }
                change_length(len_tmp);
                data.array[len_tmp - 2] = 0;
                data.array[len_tmp - 1] = 1;
            } else {
                data.array[len_tmp - 1] = tmp;
            }
        }
    }

    //快速右移一位,只能操作大小为2^n的数
    void quick_self_half() {
        size_t len_tmp = length();
        if (len_tmp > 0) {
            hint::UINT_32 tmp = data.array[len_tmp - 1];
            hint::self_half(tmp);
            if (tmp == 0) {
                len_tmp--;
                if (len_tmp == 0) {
                    return;
                }
                change_length(len_tmp);
                data.array[len_tmp - 1] = hint::HINT_INT32_0X80;
            } else {
                data.array[len_tmp - 1] = tmp;
            }
        }
    }

    //将自身的二倍返回
    HyperInt quick_twice() const {
        HyperInt result(*this);
        result.quick_self_twice();
        result.set_true_len();
        return result;
    }

    //将自身的一半返回
    HyperInt quick_half() const {
        HyperInt result(*this);
        result.quick_self_half();
        result.set_true_len();
        return result;
    }

    //基础乘法
    HyperInt normal_multiply(const HyperInt &input) const {
        if (equal_to_z() || input.equal_to_z()) {
            return HyperInt();
        }
        size_t len1 = length(), len2 = input.length();
        if (len1 <= 2) {
            return input * first_int64();
        } else if (len2 <= 2) {
            return *this * input.first_int64();
        }
        size_t result_len = len1 + len2;
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        hint::UINT_64 tmp = 0, sum = 0;
        for (size_t pos1 = 0; pos1 < len1; pos1++) {
            for (size_t pos2 = 0; pos2 < len2; pos2++) {
                tmp = static_cast<hint::UINT_64>(data.array[pos1]) * input.data.array[pos2];
                for (size_t pos3 = pos1 + pos2; pos3 < result_len; pos3++) {
                    sum = tmp + result.data.array[pos3];
                    result.data.array[pos3] = static_cast<hint::UINT_32>(sum & hint::HINT_INT32_0XFF);
                    if ((sum >> hint::HINT_INT_BIT) > 0) {
                        tmp = sum >> hint::HINT_INT_BIT;
                    } else {
                        break;
                    }
                }
            }
        }
        result.set_true_len();
        result.neg_sign(is_neg() != input.is_neg());
        return result;
    }

    //基础平方
    HyperInt normal_square() const {
        if (equal_to_z()) {
            return HyperInt();
        }
        size_t len = length();
        size_t result_len = hint::twice(len);
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        hint::UINT_64 tmp = 0, sum = 0;
        for (size_t pos1 = 0; pos1 < len; pos1++) {
            for (size_t pos2 = 0; pos2 < len; pos2++) {
                tmp = static_cast<hint::UINT_64>(data.array[pos1]) * data.array[pos2];
                for (size_t pos3 = pos1 + pos2; pos3 < result_len; pos3++) {
                    sum = tmp + result.data.array[pos3];
                    result.data.array[pos3] = static_cast<hint::UINT_32>(sum & hint::HINT_INT32_0XFF);
                    if ((sum >> hint::HINT_INT_BIT) > 0) {
                        tmp = sum >> hint::HINT_INT_BIT;
                    } else {
                        break;
                    }
                }
            }
        }
        result.set_true_len();
        result.neg_sign(false);
        return result;
    }

    //快速傅里叶变换乘法
    HyperInt fft_multiply(const HyperInt &input) const {
        if (equal_to_z() || input.equal_to_z()) {
            return HyperInt();
        }
        size_t len1 = length(), len2 = input.length();
        size_t result_len = len1 + len2;
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        size_t fft_len = 2ull << static_cast<hint::UINT_16>(ceil(log2(result_len)));
        hint::Complex *fft_ary1 = new hint::Complex[fft_len];
        // hint::Complex *fft_ary2 = nullptr;

        //每一位分解为短整数后存入复数组
        hint::UINT_16 *ary1_16 = reinterpret_cast<hint::UINT_16 *>(data.array);
        hint::UINT_16 *ary2_16 = reinterpret_cast<hint::UINT_16 *>(input.data.array);
        size_t data_len1 = len1 * 2;
        size_t data_len2 = len2 * 2;

        hint::com_ary_real_copy(fft_ary1, ary1_16, data_len1);
        hint::com_ary_img_copy(fft_ary1, ary2_16, data_len2);
        hint::fft_convolution(fft_ary1, fft_ary1, fft_ary1, fft_len, result_len > hint::HINT_FFT_NLUT_MAX);

        hint::UINT_64 tmp = 0;
        hint::UINT_16 *ary_16 = reinterpret_cast<hint::UINT_16 *>(result.data.array);
        size_t data_len = result_len * 2;

        for (size_t i = 0; i < data_len; i++) {
            tmp += static_cast<hint::UINT_64>(fft_ary1[i].imaginary / 2 + 0.5);
            ary_16[i] = static_cast<hint::UINT_16>(tmp & hint::HINT_INT16_0XFF);
            tmp >>= hint::HINT_SHORT_BIT;
        } //整理每一位

        delete[] fft_ary1;
        result.set_true_len();
        result.neg_sign(is_neg() != input.is_neg());
        return result;
    }

    // fft平方计算
    HyperInt fft_square() const {
        if (equal_to_z()) {
            return HyperInt();
        }
        size_t len = length();
        size_t result_len = hint::twice(len);
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        size_t fft_len = 2ull << static_cast<hint::UINT_16>(ceil(log2(result_len)));

        hint::Complex *fft_ary = new hint::Complex[fft_len];

        //每一位分解为短整数后存入复数组
        hint::UINT_16 *ary_16 = reinterpret_cast<hint::UINT_16 *>(data.array);
        size_t data_len = len * 2;

        hint::com_ary_real_copy(fft_ary, ary_16, data_len);
        hint::fft_convolution(fft_ary, fft_ary, fft_ary, fft_len, result_len > hint::HINT_FFT_NLUT_MAX); //卷积

        hint::UINT_64 tmp = 0;
        ary_16 = reinterpret_cast<hint::UINT_16 *>(result.data.array);
        data_len = result_len * 2;
        for (size_t i = 0; i < data_len; i++) {
            tmp += static_cast<hint::UINT_64>(fft_ary[i].real + 0.5);
            ary_16[i] = static_cast<hint::UINT_16>(tmp & hint::HINT_INT16_0XFF);
            tmp >>= hint::HINT_SHORT_BIT;
        } //整理每一位

        delete[] fft_ary;
        result.set_true_len();
        result.neg_sign(false);
        return result;
    }

    //快速数论变换乘法
    HyperInt ntt_multiply(const HyperInt &input) const {
        if (equal_to_z() || input.equal_to_z()) {
            return HyperInt();
        }
        size_t len1 = length(), len2 = input.length();
        size_t result_len = len1 + len2;
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        size_t ntt_len = 2ull << static_cast<hint::UINT_16>(ceil(log2(result_len)));
        hint::UINT_64 *ntt_ary1 = new hint::UINT_64[ntt_len * 2];
        hint::UINT_64 *ntt_ary2 = ntt_ary1 + ntt_len; // new hint::UINT_64[ntt_len];

        hint::ary_clr(ntt_ary1, ntt_len * 2);
        // hint::ary_clr(ntt_ary2, ntt_len);

        hint::UINT_16 *ary1_16 = reinterpret_cast<hint::UINT_16 *>(data.array); //每一位分解为短整数后存入数组
        hint::UINT_16 *ary2_16 = reinterpret_cast<hint::UINT_16 *>(input.data.array);
        size_t data_len1 = len1 * 2;
        size_t data_len2 = len2 * 2;
        for (size_t i = 0; i < data_len1; i++) {
            ntt_ary1[i] = ary1_16[i];
        }
        for (size_t i = 0; i < data_len2; i++) {
            ntt_ary2[i] = ary2_16[i];
        }

        hint::ntt_convolution(ntt_ary1, ntt_ary2, ntt_ary1, ntt_len);

        hint::UINT_64 tmp = 0;
        hint::UINT_16 *ary_16 = reinterpret_cast<hint::UINT_16 *>(result.data.array);
        size_t data_len = result_len * 2;
        for (size_t i = 0; i < data_len; i++) {
            tmp += ntt_ary1[i];
            ary_16[i] = static_cast<hint::UINT_16>(tmp & hint::HINT_INT16_0XFF);
            tmp >>= hint::HINT_SHORT_BIT;
        }
        //整理每一位
        delete[] ntt_ary1;
        // delete[] ntt_ary2;
        result.set_true_len();
        result.neg_sign(is_neg() != input.is_neg());
        return result;
    }

    //快速数论变换平方
    HyperInt ntt_square() const {
        if (equal_to_z()) {
            return HyperInt();
        }
        size_t len = length();
        size_t result_len = hint::twice(len);
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        size_t ntt_len = 2ull << static_cast<hint::UINT_16>(ceil(log2(result_len)));
        hint::UINT_64 *ntt_ary = new hint::UINT_64[ntt_len];
        hint::ary_clr(ntt_ary, ntt_len);

        //每一位分解为短整数后存入数组
        hint::UINT_16 *ary_16 = reinterpret_cast<hint::UINT_16 *>(data.array);
        size_t data_len = len * 2;
        for (size_t i = 0; i < data_len; i++) {
            ntt_ary[i] = ary_16[i];
        }

        hint::ntt_convolution(ntt_ary, ntt_ary, ntt_ary, ntt_len);

        hint::UINT_64 tmp = 0;
        ary_16 = reinterpret_cast<hint::UINT_16 *>(result.data.array);
        data_len = result_len * 2;
        for (size_t i = 0; i < data_len; i++) {
            tmp += ntt_ary[i];
            ary_16[i] = static_cast<hint::UINT_16>(tmp & hint::HINT_INT16_0XFF);
            tmp >>= hint::HINT_SHORT_BIT;
        } //整理每一位
        delete[] ntt_ary;
        result.set_true_len();
        result.neg_sign(false);
        return result;
    }

    // karatsuba乘法,速度较慢
    HyperInt karatsuba_multiply(const HyperInt &input) const {
        if (equal_to_z() || input.equal_to_z()) {
            return HyperInt();
        }
        size_t len1 = length(), len2 = input.length();
        size_t result_len = len1 + len2;
        if (result_len < 4) {
            return normal_multiply(input);
        }
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        size_t sub_len1, sub_len2, sub_len_public;
        if (len1 > len2) {
            sub_len1 = hint::half(len1);
            sub_len_public = len1 - sub_len1;
            sub_len2 = 0;
            if (len2 > sub_len_public) {
                sub_len2 = len2 - sub_len_public;
            }
        } else {
            sub_len2 = hint::half(len2);
            sub_len_public = len2 - sub_len2;
            sub_len1 = 0;
            if (len1 > sub_len_public) {
                sub_len1 = len1 - sub_len_public;
            }
        }
        HyperInt sub_a = split(0, sub_len_public);
        HyperInt sub_b = split(sub_len_public, sub_len1);
        HyperInt sub_c = input.split(0, sub_len_public);
        HyperInt sub_d = input.split(sub_len_public, sub_len2);

        HyperInt sub_e, sub_f, sub_g;
#ifdef MULTITHREAD
        if (hint::hint_threads > 1 && hint::cur_ths < hint::hint_threads * 2)
        {
            hint::cur_ths += 2;
            std::future<HyperInt> sub_e_th = std::async(hint_mul, sub_a, sub_c);
            std::future<HyperInt> sub_f_th = std::async(hint_mul, sub_b, sub_d);
            sub_g = (sub_a + sub_b) * (sub_c + sub_d);
            sub_e = sub_e_th.get();
            sub_f = sub_f_th.get();
            hint::cur_ths -= 2;
        }
        else
#endif
        {
            sub_e = hint_mul(sub_a, sub_c);
            sub_f = hint_mul(sub_b, sub_d);
            sub_g = hint_mul((sub_a + sub_b), (sub_c + sub_d));
        }

        size_t count = 0, pos_1 = 0, pos_2 = 0, pos_3 = 0, pos_4 = 0, len_pub = sub_len_public;
        size_t len_e = sub_e.length(), len_f = sub_f.length(), len_g = sub_g.length();
        hint::INT_64 tmp = 0;
        while (count < len_pub) {
            if (pos_1 < len_e) {
                tmp += sub_e.data.array[pos_1];
                pos_1++;
            }
            result.data.array[count] = static_cast<hint::UINT_32>(tmp + hint::HINT_INT32_0X10);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
        hint::self_twice(len_pub);
        while (count < len_pub) {
            if (pos_1 < len_e) {
                tmp += sub_e.data.array[pos_1];
                pos_1++;
            }
            if (pos_2 < len_g) {
                tmp += sub_g.data.array[pos_2];
                pos_2++;
            }
            if (pos_3 < len_e) {
                tmp -= sub_e.data.array[pos_3];
                pos_3++;
            }
            if (pos_4 < len_f) {
                tmp -= sub_f.data.array[pos_4];
                pos_4++;
            }
            result.data.array[count] = static_cast<hint::UINT_32>(tmp + hint::HINT_INT32_0X10);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
        pos_1 = 0;
        while (count < result_len) {
            if (pos_1 < len_f) {
                tmp += sub_f.data.array[pos_1];
                pos_1++;
            }
            if (pos_2 < len_g) {
                tmp += sub_g.data.array[pos_2];
                pos_2++;
            }
            if (pos_3 < len_e) {
                tmp -= sub_e.data.array[pos_3];
                pos_3++;
            }
            if (pos_4 < len_f) {
                tmp -= sub_f.data.array[pos_4];
                pos_4++;
            }
            result.data.array[count] = static_cast<hint::UINT_32>(tmp + hint::HINT_INT32_0X10);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
        result.set_true_len();
        result.neg_sign(is_neg() != input.is_neg());
        return result;
    }

    // karatsuba平方,速度较慢
    HyperInt karatsuba_square() const {
        if (equal_to_z()) {
            return HyperInt();
        }
        size_t len = length();
        size_t result_len = hint::twice(len);
        if (result_len < 4) {
            return normal_square();
        }
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        size_t sub_len, sub_len_public;
        sub_len = hint::half(len);
        sub_len_public = len - sub_len;

        HyperInt sub_a = split(0, sub_len_public);
        HyperInt sub_b = split(sub_len_public, sub_len);

        HyperInt sub_ab;
#ifdef MULTITHREAD
        if (hint::hint_threads > 1 && hint::cur_ths < hint::hint_threads * 2)
        {
            hint::cur_ths += 2;
            std::future<HyperInt> sub_a_th = std::async(hint_square, sub_a);
            std::future<HyperInt> sub_b_th = std::async(hint_square, sub_b);
            sub_ab = hint_mul(sub_a, sub_b);
            sub_a = sub_a_th.get();
            sub_b = sub_b_th.get();
            hint::cur_ths -= 2;
        }
        else
#endif
        {
            sub_ab = hint_mul(sub_a, sub_b);
            sub_a = hint_square(sub_a);
            sub_b = hint_square(sub_b);
        }

        sub_ab.self_twice();

        size_t count = 0, pos_1 = 0, pos_2 = 0, len_pub = sub_len_public;
        size_t len_aa = sub_a.length(), len_bb = sub_b.length(), len_ab = sub_ab.length();
        hint::INT_64 tmp = 0;
        while (count < len_pub) {
            if (pos_1 < len_aa) {
                tmp += sub_a.data.array[pos_1];
                pos_1++;
            }
            result.data.array[count] = static_cast<hint::UINT_32>(tmp & hint::HINT_INT32_0XFF);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
        hint::self_twice(len_pub);
        while (count < len_pub) {
            if (pos_1 < len_aa) {
                tmp += sub_a.data.array[pos_1];
                pos_1++;
            }
            if (pos_2 < len_ab) {
                tmp += sub_ab.data.array[pos_2];
                pos_2++;
            }
            result.data.array[count] = static_cast<hint::UINT_32>(tmp & hint::HINT_INT32_0XFF);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
        pos_1 = 0;
        while (count < result_len) {
            if (pos_2 < len_ab) {
                tmp += sub_ab.data.array[pos_2];
                pos_2++;
            }
            if (pos_1 < len_bb) {
                tmp += sub_b.data.array[pos_1];
                pos_1++;
            }
            result.data.array[count] = static_cast<hint::UINT_32>(tmp & hint::HINT_INT32_0XFF);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
        result.set_true_len();
        result.neg_sign(false);
        return result;
    }

    void mul_i32(hint::UINT_32 input, HyperInt &result) const {
        size_t len = length();
        result.reset_size(len + 1);
        result.change_length(len + 1);
        hint::UINT_64 sum = 0;
        for (size_t i = 0; i < len; i++) {
            hint::UINT_32 tmp = data.array[i];
            sum += (static_cast<hint::UINT_64>(tmp) * input);
            result.data.array[i] = static_cast<hint::UINT_32>(sum & hint::HINT_INT32_0XFF);
            sum >>= hint::HINT_INT_BIT;
        }
        result.data.array[len] = static_cast<hint::UINT_32>(sum & hint::HINT_INT32_0XFF);
        result.set_true_len();
    }

    void mul_i64(hint::UINT_64 input, HyperInt &result) const {
        size_t len = length();
        result.reset_size(len + 2);
        result.change_length(len + 2);

        const hint::UINT_64 input_num1 = input >> hint::HINT_INT_BIT;
        const hint::UINT_64 input_num2 = input & hint::HINT_INT32_0XFF;
        hint::UINT_32 tmp1 = 0, tmp2 = data.array[0];
        hint::UINT_64 sum1 = 0, sum2 = tmp2 * input_num2;
        result.data.array[0] = static_cast<hint::UINT_32>(sum2 & hint::HINT_INT32_0XFF);
        sum2 >>= hint::HINT_INT_BIT;
        for (size_t pos = 1; pos < len; pos++) {
            tmp1 = data.array[pos];
            sum1 += input_num1 * tmp2;
            sum2 += input_num2 * tmp1 + (sum1 & hint::HINT_INT32_0XFF);
            hint::UINT_32 sum = static_cast<hint::UINT_32>(sum2 & hint::HINT_INT32_0XFF);
            result.data.array[pos] = sum;
            sum1 >>= hint::HINT_INT_BIT;
            sum2 >>= hint::HINT_INT_BIT;
            tmp2 = tmp1;
        }
        sum1 += input_num1 * tmp2 + sum2;
        result.data.array[len] = static_cast<hint::UINT_32>(sum1 & hint::HINT_INT32_0XFF);
        result.data.array[len + 1] = static_cast<hint::UINT_32>(sum1 >> hint::HINT_INT_BIT);
        result.set_true_len();
    }

    //求倒数
    HyperInt inverse() const {
        size_t len = length();
        if (len == 0) {
            throw ("Can't divided by 0 in inv");
        }
        if (len <= 2) {
            if (len == 1) {
                return HyperInt(1).l_shift(hint::HINT_INT_BIT * 2).normal_divide(*this);
            }
            return HyperInt(1).l_shift(hint::HINT_INT_BIT * 4).normal_divide(*this);
        }
        HyperInt tmp = r_shift((len - 1) / 2 * 32);
        tmp = tmp.inverse();
        HyperInt dual = tmp.l_shift(1 + (len - 1) / 2 * 32);
        HyperInt sq = (*this * tmp.square()).r_shift((len / 2 + 1) * 64);
        dual.add_sub_inplace(sq, false);
        return dual;
    }

    //牛顿迭代法除法
    HyperInt newton_divide(const HyperInt &input) const {
        assert(!input.equal_to_z());
        hint::INT_32 cmp = abs_compare(input);
        if (cmp < 0) {
            return HyperInt();
        } else if (cmp == 0) {
            return is_neg() == input.is_neg() ? HyperInt(1) : HyperInt(-1);
        }
        size_t len1 = length(), len2 = input.length();
        size_t offset = 0;
        if (len1 > len2 * 2) {
            offset = len1 - len2 * 2;
        }
        HyperInt dividend = input.l_shift(offset * hint::HINT_INT_BIT);
        HyperInt dividend_inv = dividend.inverse();

        HyperInt result = (*this * dividend_inv).r_shift(std::max(len1, len2 * 2) * hint::HINT_INT_BIT);

        HyperInt divisor_maybe = (result + 1) * input;
        while (abs_compare(divisor_maybe) >= 0) {
            HyperInt diff = *this - divisor_maybe;
            diff = diff.normal_divide(input);
            if (diff.equal_to_z()) {
                diff = HyperInt(1);
            }
            result.add_sub_inplace(diff, true);
            divisor_maybe = (result + 1) * input;
        }
        divisor_maybe.add_sub_inplace(input, false);
        while (abs_compare(divisor_maybe) < 0) {
            HyperInt diff = divisor_maybe - *this;
            diff = diff.normal_divide(input);
            if (diff.equal_to_z()) {
                diff = HyperInt(1);
            }
            result.add_sub_inplace(diff, false);
            divisor_maybe = result * input;
        }
        result.set_true_len();
        result.neg_sign(is_neg() != input.is_neg());
        return result;
    }

    //模拟手算的除法
    HyperInt normal_divide(const HyperInt &input) const {
        assert(!input.equal_to_z());
        hint::INT_32 cmp = abs_compare(input);
        if (cmp < 0) {
            return HyperInt();
        } else if (cmp == 0) {
            return is_neg() == input.is_neg() ? HyperInt(1) : HyperInt(-1);
        }
        size_t len1 = length(), len2 = input.length();
        if (len1 <= 2) {
            HyperInt result = first_int64() / input.first_int64();
            result.neg_sign(is_neg() != input.is_neg());
            return result;
        } else if (len2 < 2) {
            HyperInt result = *this;
            result.div_mod(input.first_int32());
            result.neg_sign(is_neg() != input.is_neg());
            return result;
        }
        size_t result_len = len1 - len2 + 1;
        HyperInt result(result_len, 0);
        result.change_length(result_len);
        HyperInt dividend(*this), sub;
        size_t shift = 0;
        hint::UINT_64 tmp = 0, first_num2 = input.first_int64();
        while (dividend.abs_compare(input) >= 0) {
            shift = dividend.length() - len2;
            hint::UINT_64 first_num1 = dividend.first_int64();
            if (first_num1 > first_num2) {
                tmp = first_num1 / first_num2;
                sub = input * tmp;
                if (dividend.abs_compare(sub, shift) < 0) {
                    tmp--;
                    sub.add_sub_inplace(input, false);
                }
                dividend.add_sub_inplace(sub, false, shift);
            } else if (dividend.abs_compare(input, shift) < 0) {
                shift--;
                double digit_3 = static_cast<double>(first_num1) * static_cast<double>(hint::HINT_INT32_0X10);
                tmp = static_cast<hint::UINT_64>(digit_3 / first_num2);
                sub = input * tmp;
                dividend.add_sub_inplace(sub, false, shift);
            } else {
                dividend.add_sub_inplace(input, false, shift);
                tmp = 1;
            }
            result.data.array[shift] += static_cast<hint::UINT_32>(tmp & hint::HINT_INT32_0XFF);
            tmp >>= hint::HINT_INT_BIT;
            if (tmp > 0) {
                result.data.array[shift + 1] += static_cast<hint::UINT_32>(tmp);
            }
        }
        result.set_true_len();
        result.neg_sign(is_neg() != input.is_neg());
        return result;
    }

    HyperInt sqrt_mabye() const {
        size_t len = length();
        HyperInt mabye((len + 1) / 2, 0xffffffff);
        if (hint::is_odd(len) && len > 2) {
            double tmp = first_int64();
            tmp *= hint::HINT_INT32_0X10;
            tmp += data.array[0];
            hint::UINT_64 guess = static_cast<hint::UINT_64>(std::sqrt(tmp));
            mabye.data.array[0] = static_cast<hint::UINT_32>(guess & hint::HINT_INT32_0XFF);
            mabye.data.array[1] = static_cast<hint::UINT_32>(guess >> hint::HINT_INT_BIT);
        } else {
            mabye.data.array[0] = static_cast<hint::UINT_32>(std::sqrt(first_int32()));
        }
        return mabye;
    }

    //牛顿迭代法平方根
    HyperInt newton_sqrt() const {
        size_t len = length();
        HyperInt result(sqrt_mabye());
        HyperInt left, right;
        constexpr size_t times = 1024;
        for (size_t i = 0; i < times; i++) {
            HyperInt next = *this / result + result;
            next.self_half();
            if (next.abs_compare(result) >= 0) {
                left = std::move(result);
                right = std::move(next);
                left--;
                right++;
                break;
            }
            result = std::move(next);
        }
        while (abs_compare(left.square()) < 0) {
            left--;
        }
        while (left.abs_smaller(right)) {
            HyperInt mid = (left + right).half();
            if (left.abs_equal(mid)) {
                return left;
            }
            hint::INT_32 cmp = abs_compare(mid.square());
            if (cmp > 0) {
                left = std::move(mid);
            } else if (cmp < 0) {
                right = std::move(mid);
            } else {
                return mid;
            }
        }
        return left;
    }

    //二分法平方根
    HyperInt normal_sqrt() const {
        size_t len = length();
        HyperInt left, right;
        left.reset_size(hint::half(len) + 1);
        left.clear();
        left.change_length(hint::half(len) + 1);
        left.data.array[hint::half(len) - 1] = 1;
        left.set_true_len();
        right.reset_size(hint::half(len) + 1);
        right.clear();
        right.change_length(hint::half(len) + 1);
        right.data.array[hint::half(len) - 1] = 2;
        right.set_true_len();
        while (!abs_smaller(right.square())) {
            right.quick_self_twice();
            left.quick_self_twice();
        }
        while (left.abs_smaller(right)) {
            HyperInt mid = (left + right).half();
            if (left.abs_equal(mid)) {
                return left;
            }
            hint::INT_32 cmp = abs_compare(mid.square());
            if (cmp > 0) {
                left = std::move(mid);
            } else if (cmp < 0) {
                right = std::move(mid);
            } else {
                return mid;
            }
        }
        return left;
    }

public:
    ~HyperInt() {
        if (data.array != nullptr) {
            delete[] data.array;
            data.array = nullptr;
        }
    }

    HyperInt() {
        neg_sign(false);
        reset_size(2);
        change_length(0);
        data.array[1] = data.array[0] = 0;
    }

    //填充new_length个num进行构造
    HyperInt(size_t new_length, hint::UINT_32 num) {
        data.size = generate_size(new_length);
        change_length(new_length);
        neg_sign(false);
        if (data.array != nullptr) {
            delete[] data.array;
        }
        data.array = new hint::UINT_32[data.size];
        fill_element(num);
        set_true_len();
    }

    // HyperInt 拷贝构造
    HyperInt(const HyperInt &input) {
        if (this != &input) {
            size_t len = input.length();
            reset_size(len);
            change_length(len);
            neg_sign(input.is_neg());
            hint::ary_copy(data.array, input.data.array, len);
            set_true_len();
        }
    }

    // HyperInt 移动构造
    HyperInt(HyperInt &&input) noexcept {
        if (this != &input) {
            data.size = input.data.size;
            change_length(input.length());
            neg_sign(input.is_neg());
            delete[] data.array;
            data.array = input.data.array;
            input.data.array = nullptr;
            set_true_len();
        }
    }

    // string 参数构造
    HyperInt(const std::string &input) {
        string_in(input);
    }

    //字符串构造
    HyperInt(char input[]) {
        string_in(input);
    }

    //字符串构造
    HyperInt(const char input[]) {
        string_in(input);
    }

    //通用构造
    template<typename T>
    HyperInt(T input) {
        bool neg = hint::is_neg(input);
        hint::UINT_64 tmp = 0;
        if (neg) {
            tmp = static_cast<hint::UINT_64>(std::abs(static_cast<hint::INT_64>(input)));
        } else {
            tmp = static_cast<hint::UINT_64>(input);
        }
        data.size = 2;
        reset_size(2);
        change_length(2);
        data.array[0] = tmp & hint::HINT_INT32_0XFF;
        data.array[1] = tmp >> hint::HINT_INT_BIT;
        neg_sign(neg);
        set_true_len();
    }

    // HyperInt 拷贝赋值
    HyperInt &operator=(const HyperInt &input) {
        if (this != &input) {
            size_t len = input.length();
            reset_size(len);
            change_length(len);
            neg_sign(input.is_neg());
            hint::ary_copy(data.array, input.data.array, len);
            set_true_len();
        }
        return *this;
    }

    // HyperInt 移动赋值
    HyperInt &operator=(HyperInt &&input) noexcept {
        if (this != &input) {
            data.size = input.data.size;
            change_length(input.length());
            neg_sign(input.is_neg());
            delete[] data.array;
            data.array = input.data.array;
            input.data.array = nullptr;
            set_true_len();
        }
        return *this;
    }

    // string 赋值
    HyperInt &operator=(const std::string &input) {
        string_in(input);
        return *this;
    }

    //字符串赋值
    HyperInt &operator=(const char input[]) {
        string_in(input);
        return *this;
    }

    //字符串赋值
    HyperInt &operator=(char input[]) {
        string_in(input);
        return *this;
    }

    // 64位无符号整数赋值
    template<typename T>
    HyperInt &operator=(T input) {
        bool neg = hint::is_neg(input);
        hint::UINT_64 tmp = 0;
        if (neg) {
            tmp = static_cast<hint::UINT_64>(std::abs(static_cast<hint::INT_64>(input)));
        } else {
            tmp = static_cast<hint::UINT_64>(input);
        }
        data.size = 2;
        reset_size(2);
        change_length(2);
        data.array[0] = tmp & hint::HINT_INT32_0XFF;
        data.array[1] = tmp >> hint::HINT_INT_BIT;
        neg_sign(neg);
        set_true_len();
        return *this;
    }

    /// @brief 乘法函数
    /// @param input1
    /// @param input2
    /// @return input1和input2的乘积
    static HyperInt hint_mul(const HyperInt &input1, const HyperInt &input2) {
        if (&input1 == &input2) {
            return hint_square(input1);
        } else {
            size_t min_len = std::min(input1.length(), input2.length());
            size_t sum_len = input1.length() + input2.length();
            size_t factor = 5, fft_fac = static_cast<size_t>(sum_len * std::ceil(std::log2(sum_len)));
            if (sum_len <= hint::HINT_FFT_NLUT_MAX) {
                factor = 3;
            } else if (sum_len <= hint::HINT_FFT_LUT_MAX) {
                factor = 5;
            } else if (sum_len < hint::HINT_NTT_MAX) {
                factor = 60;
            }
            if (min_len <= factor * fft_fac / (1 + sum_len - min_len)) {
                return input1.normal_multiply(input2);
            } else if (sum_len <= hint::HINT_FFT_LUT_MAX) {
                return input1.fft_multiply(input2);
            } else if (sum_len < hint::HINT_NTT_MAX / 16) {
                return input1.karatsuba_multiply(input2);
            } else if (sum_len <= hint::HINT_NTT_MAX) {
                return input1.ntt_multiply(input2);
            } else {
                return input1.karatsuba_multiply(input2);
            }
        }
    }

    /// @brief 平方函数
    /// @param input
    /// @return input的平方
    static HyperInt hint_square(const HyperInt &input) {
        size_t len = input.length();
        if (len <= 48) {
            return input.normal_square();
        } else if (len <= hint::HINT_FFT_LUT_MAX / 2) {
            return input.fft_square();
        } else if (len < hint::HINT_NTT_MAX / 32) {
            return input.karatsuba_square();
        } else if (len <= hint::HINT_NTT_MAX / 2) {
            return input.ntt_square();
        } else {
            return input.karatsuba_square();
        }
    }

    //基本操作
    void set_true_len();                            //去除前导0
    void neg_sign(bool neg);                        //设置符号是否为负
    hint::INT_64 div_mod(hint::UINT_32 divisor);    //自身除以divisor的同时返回余数
    hint::INT_64 mod(hint::UINT_32 divisor) const;  //返回对divisor的余数
    HyperInt power(hint::UINT_64 n) const;          //快速幂
    HyperInt square() const;                        //求自身的平方
    HyperInt square_root() const;                   //求自身的平方根
    bool is_neg() const;                            //返回符号是否为为负号
    static bool is_neg(const HyperInt &input);      //返回符号是否为为负号
    size_t length() const;                          //返回长度
    size_t size() const;                            //返回分配的数组空间
    HyperInt split(size_t begin, size_t len) const; //返回从下标begi开始长度为len的子数组
    hint::INT_64 to_int64() const;                  //转hint::INT_64
    hint::UINT_64 to_uint64() const;                //转hint::UINT_64
    std::string to_string() const;                  //转string,用10进制表示的字符串
    void string_in(const std::string &str);         //由十进制string字符串输入
    void string_in(const char str[]);               //由十进制C风格字符串输入
    void normal_string_in(const std::string &str);  //输入十进制字符串，普通算法
    void quick_string_in(const std::string &str);   //输入十进制字符串，快速算法
    void console_in();                              //从控制台读入十进制值
    void print_dec() const;                         //向控制台打印十进制值
    void print_hex() const;                         //向控制台打印十六进制值

    HyperInt add_sub(const HyperInt &input,
                     bool is_add) const;                       //基础加减法a=b.add_sub(c,ture)->a=b+c;a=b.add_sub(c,fasle)->a=b-c,(b>c);
    void add_sub_inplace(const HyperInt &input, bool is_add, const size_t shift = 0); //就地加减 a+=b;a-=b,a加/减去左移位后的b，默认不移位
    void sub_inplace(const HyperInt &input);                                          //由减数调用,a.sub_inplace(b)->a=b-a;

    HyperInt &self_half();                  //自身右移一位
    HyperInt &self_twice();                 //自身左移一位
    HyperInt half() const;                  //返回右移一位后的值
    HyperInt twice() const;                 //返回左移一位后的值
    HyperInt r_shift(size_t n) const;       //右移n位
    HyperInt l_shift(size_t n) const;       //左移n位
    void reset_size(size_t new_size_input); //重新设定长度不小于new_size,1.5倍长度算法,在change_len()之前调用
    HyperInt abs() const;                   //返回绝对值
    hint::UINT_32 first_int32() const;      //返回开头一个元素转32位整数的结果
    hint::UINT_64 first_int64() const;      //返回开头两个元素转32位整数的结果

    hint::INT_32
    abs_compare(const HyperInt &input, const hint::INT_64 shift = 0) const; //自身和input移位shift比较，大于返回1，小于返回-1，等于返回0
    bool abs_larger(const HyperInt &input) const;                                        //绝对值是否大于input
    bool abs_smaller(const HyperInt &input) const;                                       //绝对值是否小于input
    bool abs_equal(const HyperInt &input) const;                                         //绝对值是否等于input
    bool equal_to_z() const;                                                             //判定是否为零
    bool is_even() const;                                                                //判断是否为偶数
    bool is_odd() const;                                                                 //判断是否为奇数

    //逻辑运算
    bool operator==(const HyperInt &input) const;

    template<typename T>
    bool operator==(T input) const;

    bool operator!=(const HyperInt &input) const;

    template<typename T>
    bool operator!=(T input) const;

    bool operator>(const HyperInt &input) const;

    template<typename T>
    bool operator>(T input) const;

    bool operator>=(const HyperInt &input) const;

    template<typename T>
    bool operator>=(T input) const;

    bool operator<(const HyperInt &input) const;

    template<typename T>
    bool operator<(T input) const;

    bool operator<=(const HyperInt &input) const;

    template<typename T>
    bool operator<=(T input) const;

    //友元函数
    friend HyperInt abs(const HyperInt &input);

    friend void print(const HyperInt &input);

    friend std::string to_string(const HyperInt &input);

    friend std::ostream &operator<<(std::ostream &output, const HyperInt &input);

    friend std::istream &operator>>(std::istream &input, HyperInt &output);

    friend HyperInt randHyperInt(size_t len); //生成长度为len的随机数
    //算术运算

    HyperInt operator+(const HyperInt &input) const;

    template<typename T>
    HyperInt operator+(T input) const;

    HyperInt operator+() const;

    HyperInt operator-(const HyperInt &input) const;

    template<typename T>
    HyperInt operator-(T input) const;

    HyperInt operator-() const;

    HyperInt operator*(const HyperInt &input) const;

    template<typename T>
    HyperInt operator*(T input) const;

    HyperInt operator/(const HyperInt &input) const;

    template<typename T>
    HyperInt operator/(T input) const;

    HyperInt operator%(const HyperInt &input) const;

    template<typename T>
    HyperInt operator%(T input) const;

    HyperInt &operator+=(const HyperInt &input);

    template<typename T>
    HyperInt &operator+=(T input);

    HyperInt &operator-=(const HyperInt &input);

    template<typename T>
    HyperInt &operator-=(T input);

    HyperInt &operator*=(const HyperInt &input);

    template<typename T>
    HyperInt &operator*=(T input);

    HyperInt &operator/=(const HyperInt &input);

    template<typename T>
    HyperInt &operator/=(T input);

    HyperInt &operator%=(const HyperInt &input);

    template<typename T>
    HyperInt &operator%=(T input);

    HyperInt operator++(int);

    HyperInt &operator++();

    HyperInt operator--(int);

    HyperInt &operator--();

    HyperInt operator~() const;

    HyperInt operator|(const HyperInt &input) const;

    HyperInt operator&(const HyperInt &input) const;

    HyperInt operator^(const HyperInt &input) const;

    HyperInt &operator|=(const HyperInt &input);

    HyperInt &operator&=(const HyperInt &input);

    HyperInt &operator^=(const HyperInt &input);
};

/// @brief 去除前导0
inline void HyperInt::set_true_len() {
    size_t t_len = length();
    while (t_len > 0 && data.array[t_len - 1] == 0) {
        t_len--;
    }
    change_length(t_len);
}

/// @brief 设置符号是否为负，输入true时为负，false为非负
/// @param neg
inline void HyperInt::neg_sign(bool neg) {
    if ((!neg) || equal_to_z()) {
        data.neg_n_len = data.neg_n_len & hint::HINT_SIZE_0X7F;
    } else {
        data.neg_n_len = data.neg_n_len | hint::HINT_SIZE_0X80;
    }
}

/// @brief 同时计算商和余数
/// @param divisor
/// @return 自身除以32位无符号整数divisor的同时返回余数
hint::INT_64 HyperInt::div_mod(hint::UINT_32 divisor) {
    lldiv_t div_tmp;
    if (divisor == 0) {
        return to_int64();
    }
    hint::UINT_64 last_rem = 0, tmp = 0, rem_num = 0;
    size_t pos = length();
    while (pos > 1) {
        pos--;
        tmp = (last_rem << hint::HINT_INT_BIT) + data.array[pos];
        div_tmp = lldiv(tmp, divisor);                              //一次性得到商和余数
        data.array[pos] = static_cast<hint::UINT_32>(div_tmp.quot); //当前数为变商
        last_rem = div_tmp.rem;                                     //得到余数
    }
    tmp = (last_rem << hint::HINT_INT_BIT) + data.array[0];
    div_tmp = lldiv(tmp, divisor);
    data.array[0] = static_cast<hint::UINT_32>(div_tmp.quot);
    rem_num = div_tmp.rem;
    set_true_len();
    return rem_num;
}

/// @brief 求余数函数
/// @param divisor
/// @return 对32位无符号整数divisor的余数
hint::INT_64 HyperInt::mod(hint::UINT_32 divisor) const {
    //  lldiv_t div_tmp;
    if (divisor == 0) {
        return to_int64();
    }
    hint::INT_64 last_rem = 0, tmp = 0, rem_num = 0;
    size_t pos = length();
    while (pos > 1) {
        pos--;
        tmp = (last_rem << hint::HINT_INT_BIT) + data.array[pos];
        auto div_tmp = lldiv(tmp, divisor); //一次性得到商和余数
        last_rem = div_tmp.rem;             //得到余数
    }
    tmp = (last_rem << hint::HINT_INT_BIT) + data.array[0];
    auto div_tmp = lldiv(tmp, divisor);
    rem_num = div_tmp.rem;
    if (is_neg()) {
        rem_num = -rem_num;
    }
    return rem_num;
}

/// @brief 快速幂
/// @param n
/// @return 自身的n次幂
inline HyperInt HyperInt::power(hint::UINT_64 n) const {
    HyperInt tmp(*this), result = HyperInt(1);
    if (!hint::is_odd(n)) {
        result.neg_sign(false);
    } else {
        result.neg_sign(is_neg());
    }
    if (abs_equal(result)) {
        return result;
    }
    while (n) {
        if (n & 1) {
            result = tmp * result;
        }
        tmp = tmp.square();
        hint::self_half(n);
    }
    return result;
}

/// @brief 平方函数
/// @return 自身的平方
inline HyperInt HyperInt::square() const {
#ifdef MULTITHREAD
    size_t len = length();
    if (hint::hint_threads > 1 && len >= hint::HINT_FFT_LUT_MAX / 3)
    {
        return karatsuba_square();
    }
    else
#endif
    {
        return hint_square(*this);
    }
}

/// @brief 平方根函数
/// @return 自身的平方根
inline HyperInt HyperInt::square_root() const {
    size_t len = length();
    if (len <= 1) {
        return HyperInt(static_cast<hint::UINT_32>(sqrt(first_int32())));
    } else if (len <= 2) {
        return normal_sqrt();
    } else {
        return newton_sqrt();
    }
}

/// @brief 判断自身符号是否为负
/// @return true为负号，false为非负
inline bool HyperInt::is_neg() const {
    return (data.neg_n_len & hint::HINT_SIZE_0X80) != 0;
}

/// @brief 判断input符号是否为负
/// @param input
/// @return true为负号，false为非负
inline bool HyperInt::is_neg(const HyperInt &input) {
    return input.is_neg();
}

/// @brief 输出内部数字长度
/// @return 内部数字长度
inline size_t HyperInt::length() const {
    return data.neg_n_len & hint::HINT_SIZE_0X7F;
}

/// @brief 输出内部数组长度
/// @return 分配的数组长度
inline size_t HyperInt::size() const {
    return data.size;
}

/// @brief 分割数字
/// @param begin
/// @param len
/// @return 从下标begin开始长度为len的子数组构成的HyperInt
inline HyperInt HyperInt::split(size_t begin, size_t len) const {
    if (len == 0) {
        return HyperInt(0);
    }
    size_t data_len = length();
    if (begin >= data_len) {
        hint::INT_64 num = data.array[data_len - 1];
        return HyperInt(num);
    } else if (len > data_len - begin) {
        len = data_len - begin;
    }
    HyperInt result;
    result.reset_size(len);
    result.change_length(len);
    hint::ary_copy(result.data.array, data.array + begin, len);
    result.set_true_len();
    result.neg_sign(is_neg());
    return result;
}

/// @brief 转64位有符号整数
/// @return 内部数组前两位代表的64位整数
inline hint::INT_64 HyperInt::to_int64() const {
    if (to_uint64() == hint::HINT_INT64_0X80) {
        return static_cast<hint::INT_64>(hint::HINT_INT64_0X80);
    }
    hint::INT_64 out = 0;
    out = (data.array[1] & hint::HINT_INT32_0X7F);
    out <<= hint::HINT_INT_BIT;
    out += data.array[0];
    if (is_neg()) {
        out = -out;
    }
    return out;
}

/// @brief 转64位无符号整数
/// @return 内部数组前两位代表的64位无符号整数
inline hint::UINT_64 HyperInt::to_uint64() const {
    hint::UINT_64 out = 0;
    out = data.array[1];
    out <<= hint::HINT_INT_BIT;
    out += data.array[0];
    return out;
}

/// @brief 转std::string字符串
/// @return 用10进制表示的字符串
std::string HyperInt::to_string() const {
    if (equal_to_z()) {
        return std::string("0");
    }
    size_t in_len = length();
    if (in_len <= 2) {
        if (is_neg()) {
            return std::string("-") + std::to_string(first_int64());
        }
        return std::to_string(first_int64());
    }
    std::string result_str;
    if (is_neg()) {
        result_str += '-';
    }
    if (in_len <= 1536) {
        HyperInt input(*this);
        hint::UINT_64 rem_n = 0;
        constexpr hint::UINT_64 factor = 1e9;
        std::stack<std::string> str_stack;
        while (input.abs_larger(HyperInt(0))) {
            rem_n = input.div_mod(factor);
            str_stack.emplace(hint::ui64to_string(rem_n, 9));
        }
        if (!str_stack.empty()) {
            str_stack.top() = std::to_string(rem_n);
        }
        while (!str_stack.empty()) {
            result_str += str_stack.top();
            str_stack.pop();
        }
    } else {
        size_t result_len = in_len * 4;
        size_t ary_len = 2ull << static_cast<hint::UINT_16>(std::ceil(std::log2(result_len)));
        hint::UINT_8 *result = new hint::UINT_8[ary_len]; //结果数组
        memcpy(result, data.array, in_len * sizeof(*data.array));
        while (result[result_len - 1] == 0) {
            result_len--;
        }

        hint::base_conversion(result, result_len, hint::HINT_INT8_0X10, 100); //进制转换

        result_str += std::to_string(static_cast<hint::UINT_32>(result[result_len - 1]));
        result_len--;
        while (result_len > 0) //整理每一位将100进制转为十进制
        {
            result_len--;
            result_str += hint::ui64to_string(static_cast<hint::UINT_64>(result[result_len]), 2);
        }
        delete[] result;
    }
    return result_str;
}

/// @brief 输入表示十进制的std::string字符串
/// @param str
inline void HyperInt::string_in(const std::string &str) {
    size_t len = str.size();
    if (len > 200000 && len <= 268435456) {
        quick_string_in(str);
    } else {
        normal_string_in(str);
    }
}

/// @brief 输入十进制字符串
/// @param str
inline void HyperInt::string_in(const char str[]) {
    string_in(std::string(str));
}

/// @brief 输入十进制字符串，慢速算法
/// @param str
inline void HyperInt::normal_string_in(const std::string &str) {
    clear();
    constexpr hint::UINT_64 factor = 1e19;
    size_t str_len = str.size(), pos = 0;
    if (str_len == 0) {
        clear();
        set_true_len();
        return;
    }
    if (str[0] == '-') {
        pos++;
    }
    while (str_len - pos >= 19) {
        *this *= factor;
        *this += hint::stoui64(std::string(str.begin() + pos, str.begin() + pos + 19));
        pos += 19;
    }
    if (str_len - pos > 0) {
        *this *= hint::qpow(10ull, str_len - pos);
        *this += hint::stoui64(std::string(str.begin() + pos, str.end()));
    }
    if (str[0] == '-') {
        neg_sign(true);
    }
}

/// @brief 输入十进制字符串，快速迭代算法
/// @param str
void HyperInt::quick_string_in(const std::string &str) {
    size_t in_len = str.size();
    if (in_len == 0) {
        clear();
        set_true_len();
        return;
    }
    size_t trans_len = in_len / 2 + 1;
    size_t ary_len = 1ull << static_cast<hint::UINT_16>(ceil(log2(trans_len)));
    hint::UINT_8 *trans_ary = new hint::UINT_8[ary_len];
    hint::ary_clr(trans_ary, ary_len);
    bool neg = (str[0] == '-');
    for (size_t pos = 0; pos < trans_len; pos++) {
        hint::UINT_16 tmp = 0, rank = 1;
        for (size_t i = 0; i < 2 && in_len > 0; i++) {
            in_len--;
            char c = str[in_len];
            if ('0' <= c && c <= '9') {
                tmp += rank * (c - '0');
            }
            rank *= 10;
        }
        trans_ary[pos] = static_cast<hint::UINT_8>(tmp);
    }
    while (trans_ary[trans_len - 1] == 0) {
        trans_len--;
    }
    hint::base_conversion(trans_ary, trans_len, 100, hint::HINT_INT8_0X10);
    reset_size(1 + trans_len / 4);
    clear();
    change_length(1 + trans_len / 4);
    memcpy(data.array, trans_ary, trans_len * sizeof(*trans_ary));
    set_true_len();
    if (!equal_to_z()) {
        neg_sign(neg);
    }
    delete[] trans_ary;
}

/// @brief 从控制台读入十进制值
inline void HyperInt::console_in() {
    clear();
    char tmp = '0';
    bool head = true, neg = false;
    while (tmp != '\n' && '0' <= tmp && tmp <= '9') {
        if (tmp != '\n') {
            *this *= static_cast<hint::UINT_64>(10);
            *this += (tmp - '0');
        }
        tmp = getchar();
        if (head) {
            if (tmp == '-') {
                neg = true;
            }
            head = false;
        }
    }
    neg_sign(neg);
}

/// @brief 向控制台打印十进制值
void HyperInt::print_dec() const {
    if (equal_to_z()) {
        printf("0");
    }
    if (is_neg()) {
        printf("-");
    }
    size_t in_len = length();
    if (in_len <= 2) {
        std::cout << first_int64();
    } else if (in_len <= 1536) {
        HyperInt input(*this);
        hint::INT_64 rem_n = 0;
        constexpr hint::UINT_64 factor = 1e9;
        std::stack<hint::UINT_32> num_stack;
        while (input.abs_larger(HyperInt(0))) {
            rem_n = input.div_mod(factor);
            num_stack.push(static_cast<hint::UINT_32>(rem_n));
        }
        if (!num_stack.empty()) {
            printf("%u", num_stack.top());
            num_stack.pop();
        }
        while (!num_stack.empty()) {
            printf("%09u", num_stack.top());
            num_stack.pop();
        }
    } else {
        size_t result_len = in_len * 4;
        size_t ary_len = 2ull << static_cast<hint::UINT_16>(std::ceil(std::log2(result_len)));
        hint::UINT_8 *result = new hint::UINT_8[ary_len]; //结果数组
        memcpy(result, data.array, in_len * sizeof(*data.array));
        while (result[result_len - 1] == 0) {
            result_len--;
        }

        hint::base_conversion(result, result_len, hint::HINT_INT8_0X10, 100);
        if (result_len > 0) {
            result_len--;
            printf("%d", result[result_len]);
        }
        while (result_len > 0) {
            result_len--;
            printf("%02d", result[result_len]);
        }

        delete[] result;
    }
    printf("\n");
}

/// @brief 向控制台打印十六进制值
inline void HyperInt::print_hex() const {
    if (equal_to_z()) {
        printf("0");
    } else {
        if (is_neg()) {
            printf("-");
        }
        size_t pos = length();
        printf("%X ", data.array[pos - 1]);
        pos--;
        while (pos > 0) {
            pos--;
            printf("%08X ", data.array[pos]);
        }
    }
    printf("\n");
}

/// @brief 自身绝对值与输入绝对值的加减
/// @param input
/// @param is_add
/// @return is_add为true时返回自身绝对值加input绝对值，为false时返回自身绝对值减input绝对值
inline HyperInt HyperInt::add_sub(const HyperInt &input, bool is_add) const {
    HyperInt result;
    size_t len1 = length(), len2 = input.length();
    if (is_add) {
        size_t result_len = std::max(len1, len2) + 1;
        result.reset_size(result_len);
        result.change_length(result_len);
        hint::INT_64 tmp = 0;
        size_t count = 0;
        while (count < result_len) {
            if (count < len1) {
                tmp += data.array[count];
            }
            if (count < len2) {
                tmp += input.data.array[count];
            }
            result.data.array[count] = static_cast<hint::UINT_32>(tmp & hint::HINT_INT32_0XFF);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
    } else {
        size_t result_len = std::max(len1, len2);
        result.reset_size(result_len);
        result.change_length(result_len);
        hint::INT_64 tmp = 0;
        size_t count = 0;
        while (count < result_len) {
            if (count < len1) {
                tmp += data.array[count];
            }
            if (count < len2) {
                tmp -= input.data.array[count];
            }
            result.data.array[count] = static_cast<hint::UINT_32>(tmp + hint::HINT_INT32_0X10);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
    }
    result.set_true_len();
    return result;
}

/// @brief 自身绝对值与input左移shift位后的绝对值相加减
/// @param input
/// @param is_add
/// @param shift
inline void HyperInt::add_sub_inplace(const HyperInt &input, bool is_add, const size_t shift) {
    size_t len1 = length(), len2 = input.length();
    if (is_add) {
        size_t result_len = std::max(len1, len2) + 1;
        reset_size(result_len);
        hint::ary_clr(data.array + len1, result_len - len1);
        change_length(result_len);
        hint::INT_64 tmp = 0;
        size_t count = shift;
        while (count < result_len) {
            hint::UINT_32 first_num = 0;
            if (count < len1) {
                first_num = data.array[count];
                tmp += first_num;
            }
            if (count - shift < len2) {
                tmp += input.data.array[count - shift];
            } else if (tmp == static_cast<hint::INT_64>(first_num)) {
                break;
            }
            data.array[count] = static_cast<hint::UINT_32>(tmp & hint::HINT_INT32_0XFF);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
    } else {

        size_t result_len = std::max(len1, len2);
        reset_size(result_len);
        change_length(result_len);
        hint::INT_64 tmp = 0;
        size_t count = shift;
        while (count < result_len) {
            hint::UINT_32 first_num = 0;
            if (count < len1) {
                first_num = data.array[count];
                tmp += first_num;
            }
            if (count - shift < len2) {
                tmp -= input.data.array[count - shift];
            } else if (tmp == static_cast<hint::INT_64>(first_num)) {
                break;
            }
            data.array[count] = static_cast<hint::UINT_32>(tmp + hint::HINT_INT32_0X10);
            tmp >>= hint::HINT_INT_BIT;
            count++;
        }
    }
    set_true_len();
}

/// @brief 自身绝对值变为input绝对值减自身绝对值
/// @param input
inline void HyperInt::sub_inplace(const HyperInt &input) {
    size_t len1 = length(), len2 = input.length();
    size_t result_len = std::max(len1, len2);
    reset_size(result_len);
    change_length(result_len);
    hint::INT_64 tmp = 0;
    size_t count = 0;
    while (count < result_len) {
        if (count < len2) {
            tmp += input.data.array[count];
        }
        if (count < len1) {
            tmp -= data.array[count];
        }
        data.array[count] = static_cast<hint::UINT_32>(tmp + hint::HINT_INT32_0X10);
        tmp >>= hint::HINT_INT_BIT;
        count++;
    }
    set_true_len();
}

/// @brief 自身左移一位，变成原来的一半
/// @return 变换后的自身
inline HyperInt &HyperInt::self_half() {
    hint::UINT_32 tmp1, tmp2 = 0;
    size_t pos = length();
    while (pos) {
        pos--;
        tmp1 = tmp2;
        tmp2 = data.array[pos];
        tmp1 = (tmp1 << (hint::HINT_INT_BIT - 1)) + (tmp2 >> 1);
        data.array[pos] = tmp1;
    }
    set_true_len();
    return *this;
}

/// @brief 自身左移一位，变成原来的二倍
/// @return 变换后的自身
inline HyperInt &HyperInt::self_twice() {
    hint::UINT_64 tmp = 0;
    size_t len = length();
    for (size_t pos = 0; pos < len; pos++) {
        tmp += (static_cast<hint::UINT_64>(data.array[pos]) << 1);
        data.array[pos] = static_cast<hint::UINT_32>(tmp);
        tmp >>= hint::HINT_INT_BIT;
    }
    if (tmp > 0) {
        len++;
        reset_size(len);
        change_length(len);
        data.array[len - 1] = hint::HINT_INT32_0X01;
    }
    return *this;
}

/// @brief 除以2
/// @return 自身的一半
inline HyperInt HyperInt::half() const {
    return r_shift(1);
}

/// @brief 乘以2
/// @return 自身的二倍
inline HyperInt HyperInt::twice() const {
    return l_shift(1);
}

/// @brief 二进制数做移n位的结果函数
/// @param n
/// @return 二进制数右移n位的结果，不改变自身的值
inline HyperInt HyperInt::r_shift(size_t n) const {
    if (n == 0) {
        return *this;
    }
    HyperInt result;
    size_t shift = n / 32;
    size_t offset = n % 32;
    size_t len = length();
    shift %= len;
    len -= shift;
    hint::UINT_32 tmp1, tmp2 = 0;
    result.reset_size(len);
    result.change_length(len);
    while (len > 0) {
        len--;
        tmp1 = tmp2;
        tmp2 = data.array[len + shift];
        tmp1 = (offset ? (tmp1 << (hint::HINT_INT_BIT - offset)) : 0) + (tmp2 >> offset);
        result.data.array[len] = tmp1;
    }
    result.neg_sign(is_neg());
    result.set_true_len();
    return result;
}

/// @brief 二进制数左移n位的结果函数
/// @param n
/// @return 二进制数左移n位的结果，不改变自身的值
inline HyperInt HyperInt::l_shift(size_t n) const {
    if (n == 0) {
        return *this;
    }
    HyperInt result;
    size_t shift = n / 32;
    size_t offset = n % 32;
    size_t len = length();
    hint::UINT_32 tmp1 = 0, tmp2 = 0;
    result.reset_size(len + shift + 1);
    result.clear();
    result.change_length(len + shift + 1);
    for (size_t pos = 0; pos < length(); pos++) {
        tmp1 = tmp2;
        tmp2 = data.array[pos];
        tmp1 = (offset ? (tmp1 >> (hint::HINT_INT_BIT - offset)) : 0) + (tmp2 << offset);
        result.data.array[pos + shift] = tmp1;
    }
    if (tmp2 >> (hint::HINT_INT_BIT - offset) & offset) {
        result.data.array[len + shift] = tmp2 >> (hint::HINT_INT_BIT - offset);
    } else {
        result.change_length(result.length() - 1);
    }
    result.neg_sign(is_neg());
    return result;
}

/// @brief 重新设定内部数组长度不小于new_size,1.5倍长度算法
/// @param new_size_input
inline void HyperInt::reset_size(size_t new_size_input) {
    size_t size_tmp = generate_size(new_size_input);
    if (data.array == nullptr) {
        data.size = size_tmp;
        data.array = new hint::UINT_32[data.size];
        change_length(0);
    } else if (data.size != size_tmp) {
        data.size = size_tmp;
        data.array = hint::ary_realloc(data.array, data.size);
        change_length(std::min(length(), data.size));
    }
}

/// @brief 绝对值函数
/// @return 绝对值
inline HyperInt HyperInt::abs() const {
    HyperInt result(*this);
    result.neg_sign(false);
    return result;
}

/// @brief 内部数组开头的32位无符号整数
/// @return 返回内部数组开头的32位无符号整数
inline hint::UINT_32 HyperInt::first_int32() const {
    if (length() >= 1) {
        return data.array[length() - 1];
    } else {
        return 0;
    }
}

/// @brief 返回内部数组开头的64位无符号整数
/// @return 内部数组开头的64位无符号整数
inline hint::UINT_64 HyperInt::first_int64() const {
    if (length() >= 2) {
        hint::UINT_64 result = data.array[length() - 1];
        result <<= hint::HINT_INT_BIT;
        result += data.array[length() - 2];
        return result;
    } else {
        return static_cast<hint::UINT_64>(first_int32());
    }
}

/// @brief 和input左移位shift比较绝对值
/// @param input
/// @param shift
/// @return 大于返回1，小于返回-1，等于返回0
inline hint::INT_32 HyperInt::abs_compare(const HyperInt &input, const hint::hint_size_t shift) const {
    hint::hint_size_t len1 = static_cast<hint::hint_size_t>(length());
    hint::hint_size_t len2 = static_cast<hint::hint_size_t>(input.length()) + shift;

    if (len1 < len2) {
        return -1;
    } else if (len1 > len2) {
        return 1;
    }
    hint::UINT_32 num1 = 0, num2 = 0;
    size_t len = len1;
    while (len > 0) {
        len--;
        num1 = data.array[len];
        num2 = input.data.array[len - shift];
        if (num1 > num2) {
            return 1;
        } else if (num1 < num2) {
            return -1;
        }
    }
    return 0;
}

/// @brief 判断绝对值是否大于input
/// @param input
/// @return true为绝对值大于input，false为绝对值小于等于input
inline bool HyperInt::abs_larger(const HyperInt &input) const {
    return abs_compare(input) > 0;
}

/// @brief 判断绝对值是否小于input
/// @param input
/// @return true为绝对值小于input，false为绝对值大于等于input
inline bool HyperInt::abs_smaller(const HyperInt &input) const {
    return abs_compare(input) < 0;
}

/// @brief 判断绝对值是否等于input
/// @param input
/// @return true为绝对值相等，false为绝对值不相等
inline bool HyperInt::abs_equal(const HyperInt &input) const {
    if (this == &input) {
        return true;
    }
    return abs_compare(input) == 0;
}

/// @brief 判断是否为零
/// @return true代表等于0，false代表不等于0
inline bool HyperInt::equal_to_z() const {
    return (length() == 0);
}

/// @brief 判断是否为偶数
/// @return true代表是偶数，false代表是奇数
inline bool HyperInt::is_even() const {
    if (length() == 0) {
        return true;
    }
    return (data.array[0] & 1) == 0;
}

/// @brief 判断是否为奇数
/// @return true代表是奇数，false代表是偶数
inline bool HyperInt::is_odd() const {
    if (length() == 0) {
        return false;
    }
    return (data.array[0] & 1) != 0;
}

inline bool HyperInt::operator==(const HyperInt &input) const {
    if (is_neg() != input.is_neg()) {
        return false;
    } else {
        return abs_equal(input);
    }
}

template<typename T>
inline bool HyperInt::operator==(T input) const {
    if (is_neg() != hint::is_neg(input)) {
        return false;
    } else {
        return abs_equal(HyperInt(input));
    }
}

inline bool HyperInt::operator!=(const HyperInt &input) const {
    if (is_neg() != input.is_neg()) {
        return true;
    } else {
        return !abs_equal(input);
    }
}

template<typename T>
inline bool HyperInt::operator!=(T input) const {
    if (is_neg() != hint::is_neg(input)) {
        return true;
    } else {
        return !abs_equal(HyperInt(input));
    }
}

inline bool HyperInt::operator>(const HyperInt &input) const {
    if (is_neg() != input.is_neg()) {
        return !is_neg();
    } else {
        return is_neg() != abs_larger(input);
    }
}

template<typename T>
inline bool HyperInt::operator>(T input) const {
    if (is_neg() != hint::is_neg(input)) {
        return !is_neg();
    } else {
        return is_neg() != abs_larger(HyperInt(input));
    }
}

inline bool HyperInt::operator>=(const HyperInt &input) const {
    return !(*this < input);
}

template<typename T>
inline bool HyperInt::operator>=(T input) const {
    return !(*this < input);
}

inline bool HyperInt::operator<(const HyperInt &input) const {
    if (is_neg() != input.is_neg()) {
        return is_neg();
    } else {
        return is_neg() != abs_smaller(input);
    }
}

template<typename T>
inline bool HyperInt::operator<(T input) const {
    if (is_neg() != hint::is_neg(input)) {
        return is_neg();
    } else {
        return is_neg() != abs_smaller(HyperInt(input));
    }
}

inline bool HyperInt::operator<=(const HyperInt &input) const {
    return !(*this > input);
}

template<typename T>
inline bool HyperInt::operator<=(T input) const {
    return !(*this > input);
}
//算术运算

inline HyperInt HyperInt::operator+(const HyperInt &input) const {
    HyperInt result;
    if (is_neg() == input.is_neg()) //是否同号
    {
        result = add_sub(input, true);
        result.neg_sign(is_neg());
    } else {
        const hint::INT_32 cmp = abs_compare(input);
        if (cmp > 0) {
            result = add_sub(input, false);
            result.neg_sign(is_neg());
        } else if (cmp < 0) {
            result = input.add_sub(*this, false);
            result.neg_sign(!is_neg());
        }
    }
    return result;
}

template<typename T>
inline HyperInt HyperInt::operator+(T input) const {
    return *this + HyperInt(input);
}

inline HyperInt HyperInt::operator+() const {
    return *this;
}

inline HyperInt HyperInt::operator-(const HyperInt &input) const {
    HyperInt result;
    if (this == &input) {
        return result;
    }
    if (is_neg() != input.is_neg()) //是否异号
    {
        result = add_sub(input, true);
        result.neg_sign(is_neg());
    } else {
        const hint::INT_32 cmp = abs_compare(input);
        if (cmp > 0) {
            result = add_sub(input, false);
            result.neg_sign(is_neg());
        } else if (cmp < 0) {
            result = input.add_sub(*this, false);
            result.neg_sign(!is_neg());
        }
    }
    return result;
}

template<typename T>
inline HyperInt HyperInt::operator-(T input) const {
    return *this - HyperInt(input);
}

inline HyperInt HyperInt::operator-() const {
    HyperInt result(*this);
    result.neg_sign(!is_neg());
    return result;
}

inline HyperInt HyperInt::operator*(const HyperInt &input) const {

#ifdef MULTITHREAD
    size_t len = std::max(length(), input.length());
    if (hint::hint_threads > 1 && len >= hint::HINT_FFT_LUT_MAX / 4)
    {
        return karatsuba_multiply(input);
    }
    else
#endif
    {
        return hint_mul(*this, input);
    }
}

template<typename T>
HyperInt HyperInt::operator*(T input) const {
    if (input == 1) {
        return *this;
    }
    if (equal_to_z() || input == 0) {
        return HyperInt();
    }
    hint::UINT_64 abs_input = 0;
    bool neg = hint::is_neg(input);
    HyperInt result;
    if (neg) {
        abs_input = static_cast<hint::UINT_64>(std::abs(static_cast<hint::INT_64>(input)));
    } else {
        abs_input = static_cast<hint::UINT_64>(input);
    }
    if (abs_input > hint::HINT_INT32_0XFF) {
        mul_i64(abs_input, result);
    } else {
        abs_input &= hint::HINT_INT32_0XFF;
        mul_i32(static_cast<hint::UINT_32>(abs_input), result);
    }
    result.neg_sign(is_neg() != neg);
    return result;
}

inline HyperInt HyperInt::operator/(const HyperInt &input) const {
    if (this == &input) {
        return HyperInt(1);
    }
    size_t len1 = length(), len2 = input.length();
    if (len1 <= 2000 || len2 <= 2000 || (len1 - len2) <= 2000) {
        return normal_divide(input);
    } else {
        return newton_divide(input);
    }
}

template<typename T>
inline HyperInt HyperInt::operator/(T input) const {
    return *this / HyperInt(input);
}

inline HyperInt HyperInt::operator%(const HyperInt &input) const {
    HyperInt result = *this / input;
    result *= input;
    result.neg_sign(false);
    result = this->abs() - result;
    result.neg_sign(is_neg());
    return result;
}

template<typename T>
inline HyperInt HyperInt::operator%(T input) const {
    return *this % HyperInt(input);
}

inline HyperInt &HyperInt::operator+=(const HyperInt &input) {
    if (!is_neg() != input.is_neg()) //是否同号
    {
        add_sub_inplace(input, true);
        neg_sign(is_neg());
    } else {
        if (abs_equal(input)) {
            *this = HyperInt(0);
        } else if (abs_larger(input)) {
            add_sub_inplace(input, false);
            neg_sign(is_neg());
        } else {
            sub_inplace(input);
            neg_sign(!is_neg());
        }
    }
    return *this;
}

template<typename T>
inline HyperInt &HyperInt::operator+=(T input) {
    return *this += HyperInt(input);
}

inline HyperInt &HyperInt::operator-=(const HyperInt &input) {
    if (is_neg() != input.is_neg()) //是否异号
    {
        add_sub_inplace(input, true);
        neg_sign(is_neg());
    } else {
        if (abs_equal(input)) {
            *this = HyperInt(0);
        } else if (abs_larger(input)) {
            add_sub_inplace(input, false);
            neg_sign(is_neg());
        } else {
            sub_inplace(input);
            neg_sign(!is_neg());
        }
    }
    return *this;
}

template<typename T>
inline HyperInt &HyperInt::operator-=(T input) {
    return *this -= HyperInt(input);
}

HyperInt &HyperInt::operator*=(const HyperInt &input) {
    if (this == &input) {
        *this = square();
    } else {
        *this = *this * input;
    }
    return *this;
}

template<typename T>
inline HyperInt &HyperInt::operator*=(T input) {
    if (input == 1) {
        return *this;
    }
    if (equal_to_z() || input == 0) {
        *this = HyperInt();
        return *this;
    }
    hint::UINT_64 abs_input = 0;
    bool neg = hint::is_neg(input);
    neg_sign(is_neg() != neg);
    if (neg) {
        abs_input = static_cast<hint::UINT_64>(std::abs(static_cast<hint::INT_64>(input)));
    } else {
        abs_input = static_cast<hint::UINT_64>(input);
    }
    if (abs_input > hint::HINT_INT32_0XFF) {
        mul_i64(abs_input, *this);
    } else {
        abs_input &= hint::HINT_INT32_0XFF;
        mul_i32(static_cast<hint::UINT_32>(abs_input), *this);
    }
    return *this;
}

HyperInt &HyperInt::operator/=(const HyperInt &input) {
    if (this == &input) {
        *this = HyperInt(1);
        return *this;
    }
    *this = *this / input;
    return *this;
}

template<typename T>
HyperInt &HyperInt::operator/=(T input) {
    return *this /= HyperInt(input);
}

HyperInt &HyperInt::operator%=(const HyperInt &input) {
    HyperInt tmp = *this / input;
    *this = *this - (tmp * input);
    return *this;
}

template<typename T>
HyperInt &HyperInt::operator%=(T input) {
    return *this %= HyperInt(input);
}

HyperInt HyperInt::operator++(int) {
    HyperInt tmp(*this);
    *this += HyperInt(1);
    return tmp;
}

HyperInt &HyperInt::operator++() {
    *this += HyperInt(1);
    return *this;
}

HyperInt HyperInt::operator--(int) {
    HyperInt tmp(*this);
    *this -= HyperInt(1);
    return tmp;
}

HyperInt &HyperInt::operator--() {
    *this -= HyperInt(1);
    return *this;
}

HyperInt HyperInt::operator~() const {
    size_t len = length();
    HyperInt result;
    result.reset_size(len);
    result.change_length(len);
    for (size_t i = 0; i < len; i++) {
        result.data.array[i] = ~data.array[i];
    }
    result.set_true_len();
    return result;
}

HyperInt HyperInt::operator|(const HyperInt &input) const {
    size_t len1 = length(), len2 = input.length();
    size_t result_len = std::max(len1, len2);
    size_t min_len = std::min(len1, len2);
    HyperInt result;
    result.reset_size(result_len);
    result.clear();
    result.change_length(result_len);
    size_t pos = 0;
    while (pos < min_len) {
        result.data.array[pos] = data.array[pos] | input.data.array[pos];
        pos++;
    }
    if (len1 > len2) {
        hint::ary_copy(result.data.array + min_len, data.array + min_len, len1 - len2);
    } else {
        hint::ary_copy(result.data.array + min_len, input.data.array + min_len, len2 - len1);
    }
    result.set_true_len();
    return result;
}

HyperInt HyperInt::operator&(const HyperInt &input) const {
    size_t len1 = length(), len2 = input.length();
    size_t result_len = std::min(len1, len2);
    HyperInt result;
    result.reset_size(result_len);
    result.clear();
    result.change_length(result_len);
    size_t pos = 0;
    while (pos < result_len) {
        result.data.array[pos] = data.array[pos] & input.data.array[pos];
        pos++;
    }
    result.set_true_len();
    return result;
}

HyperInt HyperInt::operator^(const HyperInt &input) const {
    size_t len1 = length(), len2 = input.length();
    size_t result_len = std::max(len1, len2);
    size_t min_len = std::min(len1, len2);
    HyperInt result;
    result.reset_size(result_len);
    result.clear();
    result.change_length(result_len);
    size_t pos = 0;
    while (pos < min_len) {
        result.data.array[pos] = data.array[pos] ^ input.data.array[pos];
        pos++;
    }
    if (len1 > len2) {
        hint::ary_copy(result.data.array + min_len, data.array + min_len, len1 - len2);
    } else {
        hint::ary_copy(result.data.array + min_len, input.data.array + min_len, len2 - len1);
    }
    result.set_true_len();
    return result;
}

HyperInt &HyperInt::operator|=(const HyperInt &input) {
    size_t len1 = length(), len2 = input.length();
    size_t result_len = std::max(len1, len2);
    size_t min_len = std::min(len1, len2);
    reset_size(result_len);
    change_length(result_len);
    size_t pos = 0;
    while (pos < min_len) {
        data.array[pos] |= input.data.array[pos];
        pos++;
    }
    if (len1 < len2) {
        hint::ary_copy(data.array + len1, input.data.array + len1, len2 - len1);
    }
    set_true_len();
    return *this;
}

HyperInt &HyperInt::operator&=(const HyperInt &input) {
    size_t len1 = length(), len2 = input.length();
    size_t result_len = std::min(len1, len2);
    if (len1 > len2) {
        reset_size(result_len);
        change_length(result_len);
    }
    size_t pos = 0;
    while (pos < result_len) {
        data.array[pos] &= input.data.array[pos];
        pos++;
    }
    set_true_len();
    return *this;
}

HyperInt &HyperInt::operator^=(const HyperInt &input) {
    size_t len1 = length(), len2 = input.length();
    size_t result_len = std::max(len1, len2);
    size_t min_len = std::min(len1, len2);
    reset_size(result_len);
    change_length(result_len);
    size_t pos = 0;
    while (pos < min_len) {
        data.array[pos] ^= input.data.array[pos];
        pos++;
    }
    if (len1 < len2) {
        hint::ary_copy(data.array + min_len, input.data.array + min_len, len2 - len1);
    }
    set_true_len();
    return *this;
}

/// @brief 绝对值函数
/// @param input
/// @return input的绝对值
HyperInt abs(HyperInt input) {
    input.neg_sign(false);
    return input;
}

/// @brief 向控制台打印input
/// @param input
void print(const HyperInt &input) {
    if (input.length() <= 1000000) {
        input.print_dec();
    } else {
        input.print_hex();
    }
}

// template <typename T>
// inline bool operator>(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) > input2;
// }
// template <typename T>
// inline bool operator>=(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) >= input2;
// }
// template <typename T>
// inline bool operator<(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) < input2;
// }
// template <typename T>
// inline bool operator<=(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) <= input2;
// }

// template <typename T>
// inline HyperInt operator+(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) + input2;
// }
// template <typename T>
// inline HyperInt operator-(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) + input2;
// }
// template <typename T>
// inline HyperInt operator*(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) * input2;
// }
// template <typename T>
// inline HyperInt operator/(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) / input2;
// }
// template <typename T>
// inline HyperInt operator%(const T &input1, const HyperInt &input2)
// {
//     return HyperInt(input1) % input2;
// }

// template <typename T>
// inline T &operator+=(const T &input1, const HyperInt &input2)
// {
//     return input1 += input2.to_int64();
// }
// template <typename T>
// inline T &operator-=(const T &input1, const HyperInt &input2)
// {
//     return input1 -= input2.to_int64();
// }
// template <typename T>
// inline T &operator*=(const T &input1, const HyperInt &input2)
// {
//     return input1 *= input2.to_int64();
// }
// template <typename T>
// inline T &operator/=(const T &input1, const HyperInt &input2)
// {
//     return input1 /= input2.to_int64();
// }
// template <typename T>
// inline T &operator%=(const T &input1, const HyperInt &input2)
// {
//     return input1 %= input2.to_int64();
// }

/// @brief 转std::string字符串函数
/// @param input
/// @return input用十进制表示的字符串
inline std::string to_string(const HyperInt &input) {
    return input.to_string();
}

/// @brief 平方根函数
/// @param input
/// @return input的平方根
inline HyperInt sqrt(const HyperInt &input) {
    return input.square_root();
}

std::ostream &operator<<(std::ostream &output, const HyperInt &input) {
    output << input.to_string();
    return output;
}

std::istream &operator>>(std::istream &input, HyperInt &output) {
    std::string in;
    input >> in;
    output.string_in(in);
    return input;
}

/// @brief 阶乘函数,排列数函数,m默认为1，结果为n的阶乘
/// @param n
/// @param m
/// @param rec_level
/// @return A(n,m),即n!/(n-m)!
HyperInt factorial(hint::UINT_64 n, hint::UINT_64 m = 1, const hint::UINT_32 rec_level = 0) {
    if (n < m) {
        return HyperInt(1);
    }
    hint::UINT_64 len = n - m;
    if (len < 120) {
        HyperInt result = 1;
        for (hint::UINT_64 i = m; i <= n; i++) {
            result *= i;
        }
        return result;
    }
    hint::UINT_64 mid = m + (len * 3 / 5);
#ifdef MULTITHREAD
    if (rec_level < hint::log2_threads)
    {
        std::future<HyperInt> first = std::async(factorial, mid, m, rec_level + 1);
        std::future<HyperInt> second = std::async(factorial, n, mid + 1, rec_level + 1);
        HyperInt result = first.get() * second.get();
        return result;
    }
    else
#endif
    {
        return factorial(mid, m, rec_level) * factorial(n, mid + 1, rec_level);
    }
}

/// @brief 组合数函数
/// @param n
/// @param m
/// @return C(n,m),即n!/((n-m)!m!)
HyperInt combination(hint::UINT_64 n, hint::UINT_64 m) {
    if (m > hint::half(n)) {
        return combination(n, n - m);
    } else if (n == 0 || m == 0) {
        return HyperInt(n);
    } else if (m >= n) {
        return HyperInt(1);
    }
    return factorial(n, (n - m + 1)) / factorial(m);
}

/// @brief 最大公因数函数
/// @param a
/// @param b
/// @return a和b的最大公因数
HyperInt gcd(HyperInt a, HyperInt b) {
    a.neg_sign(false);
    b.neg_sign(false);
    if (b.equal_to_z()) {
        return a;
    }
    HyperInt tmp = b;
    b = a % b;
    a = tmp;
    while (!b.equal_to_z()) {
        tmp = b;
        b = a % b;
        a = tmp;
    }
    return a;
}

/// @brief 最小公倍数函数
/// @param a
/// @param b
/// @return a和b的最小公倍数
HyperInt lcm(const HyperInt &a, const HyperInt &b) {
    return a * b / gcd(a, b);
}

/// @brief 随机数生成函数
/// @param len
/// @return 内部数组长度为len的随机数
HyperInt randHyperInt(size_t len) {
    HyperInt result;
    if (len == 0) {
        return result;
    }
    result.reset_size(len);
    result.change_length(len);
    std::random_device seed;
    std::default_random_engine rand_num(seed());
    std::uniform_int_distribution<hint::UINT_32> uni(1, UINT32_MAX);
    result.data.array[len - 1] = uni(rand_num);
    if (len == 1) {
        result.set_true_len();
        return result;
    }
    uni = std::uniform_int_distribution<hint::UINT_32>(0, UINT32_MAX);
    for (size_t i = 0; i < len; i++) {
        result.data.array[i] = uni(rand_num);
    }
    result.set_true_len();
    return result;
}

/// @brief 生成不少于n位的10进制圆周率
/// @param n
/// @return pi乘10的n次方
HyperInt pi_generator(hint::UINT_32 n) {
    HyperInt result = HyperInt(10).power(n).twice();
    HyperInt a = result / 3;
    result += a;
    hint::UINT_32 i = 2;
    while (a.abs_compare(HyperInt()) > 0) {
        a *= i;
        a /= (i * 2 + 1);
        result += a;
        i++;
    }
    return result;
}

#endif
