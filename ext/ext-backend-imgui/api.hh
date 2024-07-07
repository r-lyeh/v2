#if !CODE

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_USE_WCHAR32

#include <ctype.h>
#include <cimgui/imgui/imgui.h>
#include <cimgui/imgui/imgui_internal.h>

#include <cimgui/cimgui.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 1
#include <cimplot/cimplot.h>
//#include "inc/3rd_imPlot.hh"

#else

#define IMSPINNER_DEMO
#include "inc/3rd_imSpinner.h"

void igSpinner(unsigned mode) {
  static int hue = 0;
  static float nextdot = 0, nextdot2;

  enum { velocity = 1 };
  
  nextdot -= 0.07f;
  nextdot2 -= 0.2f * velocity;

  using namespace ImSpinner;
  enum { line = __LINE__ + 1 };
  switch(mode){case __LINE__-line: { ImSpinner::Spinner<e_st_rainbow>("Spinner", Radius{16.f}, Thickness{2.f}, Color{ImColor::HSV(++hue * 0.005f, 0.8f, 0.8f)}, Speed{8 * velocity}); }
        break; case __LINE__-line: { ImSpinner::Spinner<e_st_angle>("SpinnerAng", Radius{16.f}, Thickness{2.f}, Color{ImColor(255, 255, 255)}, BgColor{ImColor(255, 255, 255, 128)}, Speed{8 * velocity}, Angle{IM_PI}); }
        break; case __LINE__-line: { ImSpinner::Spinner<e_st_dots>("SpinnerDots", FloatPtr{&nextdot}, Radius{16}, Thickness{4}, Color{ImColor(255, 255, 255)}, Speed{1 * velocity}, Dots{12}, MiddleDots{6}, MinThickness{-1.f}); }
        break; case __LINE__-line: { ImSpinner::Spinner<e_st_ang>("SpinnerAngNoBg", Radius{16.f}, Thickness{2.f}, Color{ImColor(255, 255, 255)}, BgColor{ImColor(255, 255, 255, 0)}, Speed{6 * velocity}, Angle{IM_PI}); }
        break; case __LINE__-line: { ImSpinner::Spinner<e_st_ang>("SpinnerAng270", Radius{16.f}, Thickness{2.f}, Color{ImColor(255, 255, 255)}, BgColor{ImColor(255, 255, 255, 128)}, Speed{6 * velocity}, Angle{270.f / 360.f * 2 * IM_PI}); }
        break; case __LINE__-line: { ImSpinner::SpinnerAng("SpinnerAng270NoBg", 16, 6, ImColor(255, 255, 255), ImColor(255, 255, 255, 0), 6 * velocity, 270.f / 360.f * 2 * IM_PI ); }
        break; case __LINE__-line: { ImSpinner::SpinnerVDots("SpinnerVDots", 16, 4, ImColor(255, 255, 255), 2.7f * velocity); }
        break; case __LINE__-line: { ImGui::Dummy({10, 0}); ImSpinner::SpinnerBounceBall("SpinnerBounceBall", 16, 6, ImColor(255, 255, 255), 4 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerAngEclipse("SpinnerAng", 16, 5, ImColor(255, 255, 255), 6 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerIngYang("SpinnerIngYang", 16, 5, false, 0, ImColor(255, 255, 255), ImColor(255, 0, 0), 4 * velocity, IM_PI * 0.8f); }
        break; case __LINE__-line: { ImSpinner::SpinnerBounceDots("SpinnerBounceDots", 6, ImColor(255, 255, 255), 6 * velocity, 3); }
        break; case __LINE__-line: { ImSpinner::SpinnerFadeDots("SpinnerFadeDots", 6, ImColor(255, 255, 255), 8 * velocity, 3); }
        break; case __LINE__-line: { ImSpinner::SpinnerScaleDots("SpinnerMovingDots", 6, ImColor(255, 255, 255), 7 * velocity, 3); }
        break; case __LINE__-line: { ImSpinner::SpinnerMovingDots("SpinnerMovingDots", 6, ImColor(255, 255, 255), 30 * velocity, 3); }
        break; case __LINE__-line: { ImGui::Dummy({10, 0}); ImSpinner::SpinnerRotateDots("SpinnerRotateDots", 16, 6, ImColor(255, 255, 255), 4 * velocity, 2); }
        break; case __LINE__-line: { ImGui::Dummy({10, 0}); ImSpinner::SpinnerTwinAng("SpinnerTwinAng", 16, 16, 6, ImColor(255, 255, 255), ImColor(255, 0, 0), 4 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerClock("SpinnerClock", 16, 2, ImColor(255, 0, 0), ImColor(255, 255, 255), 4 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerIngYang("SpinnerIngYangR", 16, 5, true, 0.1f, ImColor(255, 255, 255), ImColor(255, 0, 0), 4 * velocity, IM_PI * 0.8f); }
        break; case __LINE__-line: { ImSpinner::SpinnerTwinAng180("SpinnerTwinAng", 16, 12, 4, ImColor(255, 255, 255), ImColor(255, 0, 0), 4 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerTwinAng360("SpinnerTwinAng360", 16, 11, 4, ImColor(255, 255, 255), ImColor(255, 0, 0), 4 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerIncDots("SpinnerIncDots", 16, 4, ImColor(255, 255, 255), 5.6f, 6); }
        break; case __LINE__-line: { ImSpinner::SpinnerDots("SpinnerDots", &nextdot2, 16, 4, ImColor(255, 255, 255), 0.3f, 12, 6, 0.f); }
        break; case __LINE__-line: { ImSpinner::SpinnerIncScaleDots("SpinnerIncScaleDots", 16, 4, ImColor(255, 255, 255), 6.6f, 6); }
        break; case __LINE__-line: { ImSpinner::SpinnerAng("SpinnerAng90", 16, 6, ImColor(255, 255, 255), ImColor(255, 255, 255, 128), 8.f * velocity, IM_PI / 2.f); }
        break; case __LINE__-line: { ImSpinner::SpinnerAng("SpinnerAng90", 16, 6, ImColor(255, 255, 255), ImColor(255, 255, 255, 0), 8.5f * velocity, IM_PI / 2.f); }
        break; case __LINE__-line: { ImSpinner::SpinnerFadeBars("SpinnerFadeBars", 10, ImColor(255, 255, 255), 4.8f * velocity, 3); }
        break; case __LINE__-line: { ImGui::Dummy({10, 0}); ImSpinner::SpinnerPulsar("SpinnerPulsar", 16, 2, ImColor(255, 255, 255), 1 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerIngYang("SpinnerIngYangR2", 16, 5, true, 3.f, ImColor(255, 255, 255), ImColor(255, 0, 0), 4 * velocity, IM_PI * 0.8f); }
        break; case __LINE__-line: { ImSpinner::SpinnerBarsRotateFade("SpinnerBarsRotateFade", 8, 18, 4, ImColor(255, 255, 255), 7.6f, 6); }
        break; case __LINE__-line: { ImSpinner::SpinnerFadeBars("SpinnerFadeScaleBars", 10, ImColor(255, 255, 255), 6.8f, 3, true); }
        break; case __LINE__-line: { ImGui::Dummy({10, 0}); ImSpinner::SpinnerBarsScaleMiddle("SpinnerBarsScaleMiddle", 6, ImColor(255, 255, 255), 8.8f, 3); }
        break; case __LINE__-line: { ImGui::Dummy({10, 0}); ImSpinner::SpinnerAngTwin("SpinnerAngTwin1", 16, 13, 2, ImColor(255, 0, 0), ImColor(255, 255, 255), 6 * velocity, IM_PI / 2.f); }
        break; case __LINE__-line: { ImSpinner::SpinnerAngTwin("SpinnerAngTwin2", 13, 16, 2, ImColor(255, 0, 0), ImColor(255, 255, 255), 6 * velocity, IM_PI / 2.f); }
        break; case __LINE__-line: { ImSpinner::SpinnerAngTwin("SpinnerAngTwin3", 13, 16, 2, ImColor(255, 0, 0), ImColor(255, 255, 255), 6 * velocity, IM_PI / 2.f, 2); }
        break; case __LINE__-line: { ImSpinner::SpinnerAngTwin("SpinnerAngTwin4", 16, 13, 2, ImColor(255, 0, 0), ImColor(255, 255, 255), 6 * velocity, IM_PI / 2.f, 2); }
        break; case __LINE__-line: { ImSpinner::SpinnerTwinPulsar("SpinnerTwinPulsar", 16, 2, ImColor(255, 255, 255), 0.5f * velocity, 2); }
        break; case __LINE__-line: { ImSpinner::SpinnerAngTwin("SpinnerAngTwin4", 14, 13, 3, ImColor(255, 0, 0), ImColor(0, 0, 0, 0), 5 * velocity, IM_PI / 1.5f, 2); }
        break; case __LINE__-line: { ImSpinner::SpinnerTwinBall("SpinnerTwinBall", 16, 11, 2, 2.5f, ImColor(255, 0, 0), ImColor(255, 255, 255), 6 * velocity, 2); }
        break; case __LINE__-line: { ImSpinner::SpinnerTwinBall("SpinnerTwinBall2", 15, 19, 2, 2.f, ImColor(255, 0, 0), ImColor(255, 255, 255), 6 * velocity, 3); }
        break; case __LINE__-line: { ImSpinner::SpinnerTwinBall("SpinnerTwinBall2", 16, 16, 2, 5.f, ImColor(255, 0, 0), ImColor(255, 255, 255), 5 * velocity, 1); }
        break; case __LINE__-line: { ImSpinner::SpinnerAngTriple("SpinnerAngTriple", 16, 13, 10, 1.3f, ImColor(255, 255, 255), ImColor(255, 0, 0), ImColor(255, 255, 255), 5 * velocity, 1.5f * IM_PI); }
        break; case __LINE__-line: { ImSpinner::SpinnerIncFullDots("SpinnerIncFullDots", 16, 4, ImColor(255, 255, 255), 5.6f, 4); }
        break; case __LINE__-line: { ImSpinner::SpinnerGooeyBalls("SpinnerGooeyBalls", 16, ImColor(255, 255, 255), 2.f); }
        break; case __LINE__-line: { ImSpinner::SpinnerRotateGooeyBalls("SpinnerRotateGooeyBalls2", 16, 5, ImColor(255, 255, 255), 6.f, 2); }
        break; case __LINE__-line: { ImSpinner::SpinnerRotateGooeyBalls("SpinnerRotateGooeyBalls3", 16, 5, ImColor(255, 255, 255), 6.f, 3); }
        break; case __LINE__-line: { ImSpinner::SpinnerMoonLine("SpinnerMoonLine", 16, 3, ImColor(200, 80, 0), ImColor(80, 80, 80), 5 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerArcRotation("SpinnerArcRotation", 13, 5, ImColor(255, 255, 255), 3 * velocity, 4); }
        break; case __LINE__-line: { ImSpinner::SpinnerArcFade("SpinnerArcFade", 13, 5, ImColor(255, 255, 255), 3 * velocity, 4); }
        break; case __LINE__-line: { ImSpinner::SpinnerFilling("SpinnerFilling", 16, 6, ImColor(255, 255, 255), ImColor(255, 0, 0), 4 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerTopup("SpinnerTopup", 16, 12, ImColor(255, 0, 0), ImColor(80, 80, 80), ImColor(255, 255, 255), 1 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerFadePulsar("SpinnerFadePulsar", 16, ImColor(255, 255, 255), 1.5f * velocity, 1); }
        break; case __LINE__-line: { ImSpinner::SpinnerFadePulsar("SpinnerFadePulsar2", 16, ImColor(255, 255, 255), 0.9f * velocity, 2); }
        break; case __LINE__-line: { ImSpinner::SpinnerPulsar("SpinnerPulsar", 16, 2, ImColor(255, 255, 255), 1 * velocity, false); }
        break; case __LINE__-line: { ImSpinner::SpinnerDoubleFadePulsar("SpinnerDoubleFadePulsar", 16, 2, ImColor(255, 255, 255), 2 * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerFilledArcFade("SpinnerFilledArcFade", 16, ImColor(255, 255, 255), 4 * velocity, 4); }
        break; case __LINE__-line: { ImSpinner::SpinnerFilledArcFade("SpinnerFilledArcFade6", 16, ImColor(255, 255, 255), 6 * velocity, 6); }
        break; case __LINE__-line: { ImSpinner::SpinnerFilledArcFade("SpinnerFilledArcFade6", 16, ImColor(255, 255, 255), 8 * velocity, 12); }
        break; case __LINE__-line: { ImSpinner::SpinnerFilledArcColor("SpinnerFilledArcColor", 16, ImColor(255, 0, 0), ImColor(255, 255, 255), 2.8f * velocity, 4); }
        break; case __LINE__-line: { ImSpinner::SpinnerCircleDrop("SpinnerCircleDrop", 16, 1.5f, 4.f, ImColor(255, 0, 0), ImColor(255, 255, 255), 2.8f * velocity, IM_PI); }
        break; case __LINE__-line: { ImSpinner::SpinnerSurroundedIndicator("SpinnerSurroundedIndicator", 16, 5, ImColor(0, 0, 0), ImColor(255, 255, 255), 7.8f * velocity); }
        break; case __LINE__-line: { ImSpinner::SpinnerTrianglesSeletor("SpinnerTrianglesSeletor", 16, 8, ImColor(0, 0, 0), ImColor(255, 255, 255), 4.8f * velocity, 8); }
        break; case __LINE__-line: { ImSpinner::SpinnerFlowingGradient("SpinnerFlowingFradient", 16, 6, ImColor(200, 80, 0), ImColor(80, 80, 80), 5 * velocity, IM_PI * 2.f); }
        break; case __LINE__-line: { ImSpinner::SpinnerRotateSegments("SpinnerRotateSegments", 16, 4, ImColor(255, 255, 255), 3 * velocity, 4); }
        break; case __LINE__-line: { ImSpinner::SpinnerRotateSegments("SpinnerRotateSegments2", 16, 3, ImColor(255, 255, 255), 2.4f * velocity, 4, 2); }
        break; case __LINE__-line: { ImSpinner::SpinnerRotateSegments("SpinnerRotateSegments3", 16, 2, ImColor(255, 255, 255), 2.1f * velocity, 4, 3); }
        break; case __LINE__-line: { ImSpinner::SpinnerLemniscate("SpinnerLemniscate", 20, 3, ImColor(255, 255, 255), 2.1f * velocity, 3); }
        break; case __LINE__-line: { ImSpinner::SpinnerRotateGear("SpinnerRotateGear", 16, 6, ImColor(255, 255, 255), 2.1f * velocity, 8); }
        default:break;
  };
}

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, we are using a more C++ like approach of declaring a class to hold both data and functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        //IMGUI_DEMO_MARKER("Examples/Console");
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("Welcome to Dear ImGui!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            ImGui::MemFree(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = ImGui::MemAlloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            ImGui::MemFree(Items[i]);
        Items.clear();
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void    Draw(const char* title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped(
            "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        ImGui::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (ImGui::SmallButton("Add Debug Text"))  { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear"))           { ClearLog(); }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

        ImGui::Separator();

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Options, Filter
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear")) ClearLog();
                ImGui::EndPopup();
            }

            // Display every line as a separate entry so we can change their color or add custom widgets.
            // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
            // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
            // to only process visible items. The clipper will automatically measure the height of your first item and then
            // "seek" to display only items in the visible area.
            // To use the clipper we can replace your standard loop:
            //      for (int i = 0; i < Items.Size; i++)
            //   With:
            //      ImGuiListClipper clipper;
            //      clipper.Begin(Items.Size);
            //      while (clipper.Step())
            //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            // - That your items are evenly spaced (same height)
            // - That you have cheap random access to your elements (you can access them given their index,
            //   without processing all the ones before)
            // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
            // We would need random-access on the post-filtered list.
            // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
            // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
            // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
            // to improve this example code!
            // If your items are of variable height:
            // - Split them into same height items would be simpler and facilitate random-seeking into your list.
            // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            if (copy_to_clipboard)
                ImGui::LogToClipboard();
            for (const char* item : Items)
            {
                if (!Filter.PassFilter(item))
                    continue;

                // Normally you would store more information in your item than just a string.
                // (e.g. make Items[] an array of structure, store color/type etc.)
                ImVec4 color;
                bool has_color = false;
                if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
                else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
                if (has_color)
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item);
                if (has_color)
                    ImGui::PopStyleColor();
            }
            if (copy_to_clipboard)
                ImGui::LogFinish();

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            ScrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                ImGui::MemFree(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        }
        else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        }
        else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(ImGuiInputTextCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackCompletion:
            {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char* word_end = data->Buf + data->CursorPos;
                const char* word_start = word_end;
                while (word_start > data->Buf)
                {
                    const char c = word_start[-1];
                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char*> candidates;
                for (int i = 0; i < Commands.Size; i++)
                    if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                        candidates.push_back(Commands[i]);

                if (candidates.Size == 0)
                {
                    // No match
                    AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
                }
                else if (candidates.Size == 1)
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0]);
                    data->InsertChars(data->CursorPos, " ");
                }
                else
                {
                    // Multiple matches. Complete as much as we can..
                    // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                    int match_len = (int)(word_end - word_start);
                    for (;;)
                    {
                        int c = 0;
                        bool all_candidates_matches = true;
                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            if (i == 0)
                                c = toupper(candidates[i][match_len]);
                            else if (c == 0 || c != toupper(candidates[i][match_len]))
                                all_candidates_matches = false;
                        if (!all_candidates_matches)
                            break;
                        match_len++;
                    }

                    if (match_len > 0)
                    {
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                    }

                    // List matches
                    AddLog("Possible matches:\n");
                    for (int i = 0; i < candidates.Size; i++)
                        AddLog("- %s\n", candidates[i]);
                }

                break;
            }
        case ImGuiInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if (data->EventKey == ImGuiKey_UpArrow)
                {
                    if (HistoryPos == -1)
                        HistoryPos = History.Size - 1;
                    else if (HistoryPos > 0)
                        HistoryPos--;
                }
                else if (data->EventKey == ImGuiKey_DownArrow)
                {
                    if (HistoryPos != -1)
                        if (++HistoryPos >= History.Size)
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != HistoryPos)
                {
                    const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, history_str);
                }
            }
        }
        return 0;
    }
};

// Helpers macros
// We normally try to not use many helpers in imgui_demo.cpp in order to make code easier to copy and paste,
// but making an exception here as those are largely simplifying code...
// In other imgui sources we can use nicer internal functions from imgui_internal.h (ImMin/ImMax) but not in the demo.
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

// Extra functions to add deletion support to ImGuiSelectionBasicStorage
struct ExampleSelectionWithDeletion : ImGuiSelectionBasicStorage
{
    // Find which item should be Focused after deletion.
    // Call _before_ item submission. Retunr an index in the before-deletion item list, your item loop should call SetKeyboardFocusHere() on it.
    // The subsequent ApplyDeletionPostLoop() code will use it to apply Selection.
    // - We cannot provide this logic in core Dear ImGui because we don't have access to selection data.
    // - We don't actually manipulate the ImVector<> here, only in ApplyDeletionPostLoop(), but using similar API for consistency and flexibility.
    // - Important: Deletion only works if the underlying ImGuiID for your items are stable: aka not depend on their index, but on e.g. item id/ptr.
    // FIXME-MULTISELECT: Doesn't take account of the possibility focus target will be moved during deletion. Need refocus or scroll offset.
    int ApplyDeletionPreLoop(ImGuiMultiSelectIO* ms_io, int items_count)
    {
        if (Size == 0)
            return -1;

        // If focused item is not selected...
        const int focused_idx = (int)ms_io->NavIdItem;  // Index of currently focused item
        if (ms_io->NavIdSelected == false)  // This is merely a shortcut, == Contains(adapter->IndexToStorage(items, focused_idx))
        {
            ms_io->RangeSrcReset = true;    // Request to recover RangeSrc from NavId next frame. Would be ok to reset even when NavIdSelected==true, but it would take an extra frame to recover RangeSrc when deleting a selected item.
            return focused_idx;             // Request to focus same item after deletion.
        }

        // If focused item is selected: land on first unselected item after focused item.
        for (int idx = focused_idx + 1; idx < items_count; idx++)
            if (!Contains(GetStorageIdFromIndex(idx)))
                return idx;

        // If focused item is selected: otherwise return last unselected item before focused item.
        for (int idx = IM_MIN(focused_idx, items_count) - 1; idx >= 0; idx--)
            if (!Contains(GetStorageIdFromIndex(idx)))
                return idx;

        return -1;
    }

    // Rewrite item list (delete items) + update selection.
    // - Call after EndMultiSelect()
    // - We cannot provide this logic in core Dear ImGui because we don't have access to your items, nor to selection data.
    template<typename ITEM_TYPE>
    void ApplyDeletionPostLoop(ImGuiMultiSelectIO* ms_io, ImVector<ITEM_TYPE>& items, int item_curr_idx_to_select)
    {
        // Rewrite item list (delete items) + convert old selection index (before deletion) to new selection index (after selection).
        // If NavId was not part of selection, we will stay on same item.
        ImVector<ITEM_TYPE> new_items;
        new_items.reserve(items.Size - Size);
        int item_next_idx_to_select = -1;
        for (int idx = 0; idx < items.Size; idx++)
        {
            if (!Contains(GetStorageIdFromIndex(idx)))
                new_items.push_back(items[idx]);
            if (item_curr_idx_to_select == idx)
                item_next_idx_to_select = new_items.Size - 1;
        }
        items.swap(new_items);

        // Update selection
        Clear();
        if (item_next_idx_to_select != -1 && ms_io->NavIdSelected)
            SetItemSelected(GetStorageIdFromIndex(item_next_idx_to_select), true);
    }
};

#pragma comment(lib, "imgui")
#endif // CODE

