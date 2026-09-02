#pragma once

#include "../UEGameProfile.hpp"
using namespace UEMemory;

// com.tzyz.gmzz —— UE5 (libUnreal.so) 专用 Profile。
//
// 定位方式: GUObjectArray 是固定偏移的全局变量,直接给地址
//   GetUnrealELF().base() + 0xF64A920
// 而不是走基类的段反扫锚点算法。
//
// 其余假设(与 AutoFix 对 libUnreal.so 的处理一致,见 AutoFix.cpp):
//   - UE5_03 默认布局, FNamePool = true
//   - isUsingOutlineNumberName = true (UE5.1+ 多数开启;若名字读出乱码优先改这里)
//   - isUsingCasePreservingName = false
// Matrix/Physx/FrameCount 未逆向,返回 0(只影响最终报告缺这三项,不影响 dump 主流程)。
class GMZZProfile : public IGameProfile
{
public:
    GMZZProfile() = default;

    bool ArchSupprted() const override
    {
        auto e_machine = GetUnrealELF().header().e_machine;
        // only arm64
        return e_machine == EM_AARCH64;
    }

    std::string GetAppName() const override
    {
        return "GMZZ(CN)";
    }

    std::vector<std::string> GetAppIDs() const override
    {
        return {"com.tzyz.gmzz"};
    }

    bool isUsingCasePreservingName() const override
    {
        return false;
    }

    bool IsUsingFNamePool() const override
    {
        return true;
    }

    bool isUsingOutlineNumberName() const override
    {
        return true;
    }

    uintptr_t GetGUObjectArrayPtr() const override
    {
        // GUObjectArray = libUnreal.so + 0xF64A920 (固定全局变量,引擎更新需复核)
        return GetUEVars()->GetBaseAddress() + 0xF64A920;
    }

    // GNames/NamePool 未加密,走基类通用反扫即可。
    uintptr_t GetNamesPtr() const override
    {
        return IGameProfile::GetNamesPtr();
    }

    uintptr_t GetFrameCount() const override
    {
        return 0;
    }

    uintptr_t GetMatrix() const override
    {
        return 0;
    }

    uintptr_t GetPhysx() const override
    {
        return 0;
    }

    UE_Offsets *GetOffsets() const override
    {
        static UE_Offsets offsets = UE_DefaultOffsets::UE5_03(isUsingCasePreservingName(),
                                                              isUsingOutlineNumberName());
        // 布局差异可由运行时 BootstrapCoreObjectArrayOffsets 自动校正;
        // 若 UObject/字段仍对不上,再在这里按 DeltaForce.hpp 的方式手动校准。
        return &offsets;
    }
};
