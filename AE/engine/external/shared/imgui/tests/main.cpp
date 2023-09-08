
#ifndef AE_ENABLE_IMGUI
#   error AE_ENABLE_IMGUI required
#endif

#include "imgui.h"
#include "imgui_internal.h"

#ifdef ANDROID
# include "TestHelper.h"

extern void AE_imgui_Test ()
#else
int main ()
#endif
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::DestroyContext(GImGui);
}
