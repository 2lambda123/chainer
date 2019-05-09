#include "chainerx/native/native_device.h"

#include <cmath>
#include <cstdint>

#include "chainerx/array.h"
#include "chainerx/device.h"
#include "chainerx/dtype.h"
#include "chainerx/kernels/math.h"
#include "chainerx/native/elementwise.h"
#include "chainerx/native/kernel_regist.h"
#include "chainerx/numeric.h"
#include "chainerx/routines/math.h"

namespace chainerx {
namespace native {
namespace {

class NativePowKernel : public PowKernel {
public:
    void Call(const Array& x1, const Array& x2, const Array& out) override {
        x1.device().CheckDevicesCompatible(x1, x2, out);
        VisitNumericDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T x2, T& out) { out = chainerx::Pow(x1, x2); }
            };
            Elementwise<const T, const T, T>(Impl{}, x1, x2, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(PowKernel, NativePowKernel);

class NativePowASKernel : public PowASKernel {
public:
    void Call(const Array& x1, Scalar x2, const Array& out) override {
        x1.device().CheckDevicesCompatible(x1, out);
        VisitNumericDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T& out) { out = chainerx::Pow(x1, x2); }
                T x2;
            };
            Elementwise<const T, T>(Impl{static_cast<T>(x2)}, x1, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(PowASKernel, NativePowASKernel);

class NativePowSAKernel : public PowSAKernel {
public:
    void Call(Scalar x1, const Array& x2, const Array& out) override {
        x2.device().CheckDevicesCompatible(x2, out);
        VisitNumericDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T& out) { out = chainerx::Pow(x2, x1); }
                T x2;
            };
            Elementwise<const T, T>(Impl{static_cast<T>(x1)}, x2, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(PowSAKernel, NativePowSAKernel);

class NativeSquareKernel : public SquareKernel {
public:
    void Call(const Array& x, const Array& out) override {
        x.device().CheckDevicesCompatible(x, out);
        VisitFloatingPointDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x, T& out) { out = x * x; }
            };
            Elementwise<const T, T>(Impl{}, x, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(SquareKernel, NativeSquareKernel);

class NativeSqrtKernel : public SqrtKernel {
public:
    void Call(const Array& x, const Array& out) override {
        x.device().CheckDevicesCompatible(x, out);
        const Array& x_cast = x.dtype() == out.dtype() ? x : x.AsType(out.dtype());
        VisitFloatingPointDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x, T& out) { out = chainerx::Sqrt(x); }
            };
            Elementwise<const T, T>(Impl{}, x_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(SqrtKernel, NativeSqrtKernel);

class NativeIsNanKernel : public IsNanKernel {
public:
    void Call(const Array& x, const Array& out) override {
        x.device().CheckDevicesCompatible(x, out);
        VisitDtype(x.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x, bool& out) { out = chainerx::IsNan(x); }
            };
            Elementwise<const T, bool>(Impl{}, x, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(IsNanKernel, NativeIsNanKernel);

class NativeIsInfKernel : public IsInfKernel {
public:
    void Call(const Array& x, const Array& out) override {
        x.device().CheckDevicesCompatible(x, out);
        VisitDtype(x.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x, bool& out) { out = chainerx::IsInf(x); }
            };
            Elementwise<const T, bool>(Impl{}, x, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(IsInfKernel, NativeIsInfKernel);

class NativeIsFiniteKernel : public IsFiniteKernel {
public:
    void Call(const Array& x, const Array& out) override {
        x.device().CheckDevicesCompatible(x, out);
        VisitDtype(x.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x, bool& out) { out = !(chainerx::IsInf(x) || chainerx::IsNan(x)); }
            };
            Elementwise<const T, bool>(Impl{}, x, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(IsFiniteKernel, NativeIsFiniteKernel);

class NativeCeilKernel : public CeilKernel {
public:
    void Call(const Array& x, const Array& out) override {
        Device& device = x.device();
        device.CheckDevicesCompatible(x, out);
        const Array& x_cast = x.dtype() == out.dtype() ? x : x.AsType(out.dtype());
        VisitFloatingPointDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x, T& out) { out = chainerx::Ceil(x); }
            };
            Elementwise<const T, T>(Impl{}, x_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(CeilKernel, NativeCeilKernel);

class NativeFloorKernel : public FloorKernel {
public:
    void Call(const Array& x, const Array& out) override {
        Device& device = x.device();
        device.CheckDevicesCompatible(x, out);
        const Array& x_cast = x.dtype() == out.dtype() ? x : x.AsType(out.dtype());
        VisitFloatingPointDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x, T& out) { out = chainerx::Floor(x); }
            };
            Elementwise<const T, T>(Impl{}, x_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_KERNEL(FloorKernel, NativeFloorKernel);

}  // namespace
}  // namespace native
}  // namespace chainerx
