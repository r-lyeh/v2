// The MIT License (MIT)
// 
// Copyright (c) 2021 hnOsmium0001
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

//#include <imgui.h>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

// TODO support std::string_view
// TODO support function pointer callback in addition to std::function

enum ImCmdTextType
{
    ImCmdTextType_Regular,
    ImCmdTextType_Highlight,
    ImCmdTextType_COUNT,
};

enum ImCmdTextFlag
{
    /// Whether the text is underlined. Default false.
    ImCmdTextFlag_Underline,
    ImCmdTextFlag_COUNT,
};

namespace ImCmd
{
struct Command
{
    std::string Name;
    std::function<void()> InitialCallback;
    std::function<void(int selected_option)> SubsequentCallback;
    std::function<void()> TerminatingCallback;
};

// Initialization
struct Context;

/// Create a new context object. If there is currently no context bound, it will also be bound as the current context.
Context* CreateContext();
/// Destroys the currently bound context.
void DestroyContext();
void DestroyContext(Context* context);

void SetCurrentContext(Context* context);
Context* GetCurrentContext();

// Command management
void AddCommand(Command command);
void RemoveCommand(const char* name);

// Styling
bool GetStyleFlag(ImCmdTextType type, ImCmdTextFlag flag);
void SetStyleFlag(ImCmdTextType type, ImCmdTextFlag flag, bool enabled);
ImFont* GetStyleFont(ImCmdTextType type);
void SetStyleFont(ImCmdTextType type, ImFont* font);
ImU32 GetStyleColor(ImCmdTextType type);
void SetStyleColor(ImCmdTextType type, ImU32 color);
void ClearStyleColor(ImCmdTextType type); //< Clear the style color for the given type, defaulting to ImGuiCol_Text

// Command palette widget
void SetNextCommandPaletteSearch(const char* text);
void SetNextCommandPaletteSearchBoxFocused();
void CommandPalette(const char* name);
bool IsAnyItemSelected();

void RemoveCache(const char* name);
void RemoveAllCaches();

// Command palette widget in a window helper
void SetNextWindowAffixedTop(ImGuiCond cond = 0);
void CommandPaletteWindow(const char* name, bool* p_open);

// Command responses, only call these in command callbacks (except TerminatingCallback)
void Prompt(std::vector<std::string> options);

} // namespace ImCmd

// Adapted from https://github.com/forrestthewoods/lib_fts/blob/master/code/fts_fuzzy_match.h
#pragma once

#include <cstdint>

namespace ImCmd
{

bool FuzzySearch(char const* pattern, char const* src, int& outScore);
bool FuzzySearch(char const* pattern, char const* src, int& outScore, uint8_t matches[], int maxMatches, int& outMatches);

} // namespace ImCmd

#include <cctype>
#include <cstring>

namespace ImCmd
{

namespace
{
    bool FuzzySearchRecursive(const char* pattern, const char* src, int& outScore, const char* strBegin, const uint8_t srcMatches[], uint8_t newMatches[], int maxMatches, int& nextMatch, int& recursionCount, int recursionLimit);
} // namespace

bool FuzzySearch(char const* pattern, char const* haystack, int& outScore)
{
    uint8_t matches[256];
    int matchCount = 0;
    return FuzzySearch(pattern, haystack, outScore, matches, sizeof(matches), matchCount);
}

bool FuzzySearch(char const* pattern, char const* haystack, int& outScore, uint8_t matches[], int maxMatches, int& outMatches)
{
    int recursionCount = 0;
    int recursionLimit = 10;
    int newMatches = 0;
    bool result = FuzzySearchRecursive(pattern, haystack, outScore, haystack, nullptr, matches, maxMatches, newMatches, recursionCount, recursionLimit);
    outMatches = newMatches;
    return result;
}

namespace
{
    bool FuzzySearchRecursive(const char* pattern, const char* src, int& outScore, const char* strBegin, const uint8_t srcMatches[], uint8_t newMatches[], int maxMatches, int& nextMatch, int& recursionCount, int recursionLimit)
    {
        // Count recursions
        ++recursionCount;
        if (recursionCount >= recursionLimit) {
            return false;
        }

        // Detect end of strings
        if (*pattern == '\0' || *src == '\0') {
            return false;
        }

        // Recursion params
        bool recursiveMatch = false;
        uint8_t bestRecursiveMatches[256];
        int bestRecursiveScore = 0;

        // Loop through pattern and str looking for a match
        bool firstMatch = true;
        while (*pattern != '\0' && *src != '\0') {
            // Found match
            if (tolower(*pattern) == tolower(*src)) {
                // Supplied matches buffer was too short
                if (nextMatch >= maxMatches) {
                    return false;
                }

                // "Copy-on-Write" srcMatches into matches
                if (firstMatch && srcMatches) {
                    memcpy(newMatches, srcMatches, nextMatch);
                    firstMatch = false;
                }

                // Recursive call that "skips" this match
                uint8_t recursiveMatches[256];
                int recursiveScore;
                int recursiveNextMatch = nextMatch;
                if (FuzzySearchRecursive(pattern, src + 1, recursiveScore, strBegin, newMatches, recursiveMatches, sizeof(recursiveMatches), recursiveNextMatch, recursionCount, recursionLimit)) {
                    // Pick the best recursive score
                    if (!recursiveMatch || recursiveScore > bestRecursiveScore) {
                        memcpy(bestRecursiveMatches, recursiveMatches, 256);
                        bestRecursiveScore = recursiveScore;
                    }
                    recursiveMatch = true;
                }

                // Advance
                newMatches[nextMatch++] = (uint8_t)(src - strBegin);
                ++pattern;
            }
            ++src;
        }

        // Determine if full pattern was matched
        bool matched = *pattern == '\0';

        // Calculate score
        if (matched) {
            const int sequentialBonus = 15; // bonus for adjacent matches
            const int separatorBonus = 30; // bonus if match occurs after a separator
            const int camelBonus = 30; // bonus if match is uppercase and prev is lower
            const int firstLetterBonus = 15; // bonus if the first letter is matched

            const int leadingLetterPenalty = -5; // penalty applied for every letter in str before the first match
            const int maxLeadingLetterPenalty = -15; // maximum penalty for leading letters
            const int unmatchedLetterPenalty = -1; // penalty for every letter that doesn't matter

            // Iterate str to end
            while (*src != '\0') {
                ++src;
            }

            // Initialize score
            outScore = 100;

            // Apply leading letter penalty
            int penalty = leadingLetterPenalty * newMatches[0];
            if (penalty < maxLeadingLetterPenalty) {
                penalty = maxLeadingLetterPenalty;
            }
            outScore += penalty;

            // Apply unmatched penalty
            int unmatched = (int)(src - strBegin) - nextMatch;
            outScore += unmatchedLetterPenalty * unmatched;

            // Apply ordering bonuses
            for (int i = 0; i < nextMatch; ++i) {
                uint8_t currIdx = newMatches[i];

                if (i > 0) {
                    uint8_t prevIdx = newMatches[i - 1];

                    // Sequential
                    if (currIdx == (prevIdx + 1))
                        outScore += sequentialBonus;
                }

                // Check for bonuses based on neighbor character value
                if (currIdx > 0) {
                    // Camel case
                    char neighbor = strBegin[currIdx - 1];
                    char curr = strBegin[currIdx];
                    if (::islower(neighbor) && ::isupper(curr)) {
                        outScore += camelBonus;
                    }

                    // Separator
                    bool neighborSeparator = neighbor == '_' || neighbor == ' ';
                    if (neighborSeparator) {
                        outScore += separatorBonus;
                    }
                } else {
                    // First letter
                    outScore += firstLetterBonus;
                }
            }
        }

        // Return best result
        if (recursiveMatch && (!matched || bestRecursiveScore > outScore)) {
            // Recursive score is better than "this"
            memcpy(newMatches, bestRecursiveMatches, maxMatches);
            outScore = bestRecursiveScore;
            return true;
        } else if (matched) {
            // "this" score is better than recursive
            return true;
        } else {
            // no match
            return false;
        }
    }
} // namespace
} // namespace ImCmd

// NOTE: we are putting these at the top, so that IMGUI_DEFINE_MATH_OPERATORS can correctly propagate for this translation unit
// NOTE: imgui marks the operators as `static`, so there are no ODR violations here
//#define IMGUI_DEFINE_MATH_OPERATORS
//#include <imgui.h>
//#include <imgui_internal.h>

//#include "imcmd_command_palette.h"
//#include "imcmd_fuzzy_search.h"

#include <algorithm>
#include <cstddef>
// NOTE: we try to use as much ImGui's helpers as possible, in order to reduce
// work if the end user decide to swap out some standard library functions for
// their own.
#include <cstring>
#include <limits>
#include <utility>

namespace ImCmd
{
// =================================================================
// Private forward decls
// =================================================================

struct StackFrame;
class ExecutionManager;

struct SearchResult;
class SearchManager;

struct CommandOperationRegister;
struct CommandOperationUnregister;
struct CommandOperation;
struct Context;

struct ItemExtraData;
struct Instance;

// =================================================================
// Private interface
// =================================================================

struct StackFrame
{
    std::vector<std::string> Options;
    int SelectedOption = -1;
};

class ExecutionManager
{
private:
    Instance* m_Instance;
    Command* m_ExecutingCommand = nullptr;
    std::vector<StackFrame> m_CallStack;

public:
    ExecutionManager(Instance& instance)
        : m_Instance{ &instance } {}

    int GetItemCount() const;
    const char* GetItem(int idx) const;
    void SelectItem(int idx);

    void PushOptions(std::vector<std::string> options);
};

struct SearchResult
{
    int ItemIndex;
    int Score;
    int MatchCount;
    uint8_t Matches[32];
};

class SearchManager
{
private:
    Instance* m_Instance;

public:
    std::vector<SearchResult> SearchResults;
    char SearchText[(std::numeric_limits<uint8_t>::max)() + 1 /* for null terminator */] = {};

public:
    SearchManager(Instance& instance)
        : m_Instance{ &instance }
    {
    }

    int GetItemCount() const;
    const char* GetItem(int idx) const;

    bool IsActive() const;

    void SetSearchText(const char* text);
    void ClearSearchText();
    void RefreshSearchResults();
};

struct CommandOperationRegister
{
    Command Candidate;
};

struct CommandOperationUnregister
{
    const char* Name;
};

struct CommandOperation
{
    enum OpType
    {
        OpType_Register,
        OpType_Unregister,
    };

    OpType Type;
    int Index;
};

struct Context
{
    ImGuiStorage Instances;
    Instance* CurrentCommandPalette = nullptr;
    std::vector<Command> Commands;
    std::vector<CommandOperationRegister> PendingRegisterOps;
    std::vector<CommandOperationUnregister> PendingUnregisterOps;
    std::vector<CommandOperation> PendingOps;
    ImFont* TextStyleFonts[ImCmdTextType_COUNT] = {};
    ImU32 TextStyleColors[ImCmdTextType_COUNT] = {};
    ImU32 TextStyleFlags[ImCmdTextType_COUNT] = {};
    int CommandStorageLocks = 0;
    bool TextStyleHasColorOverride[ImCmdTextType_COUNT] = {};
    bool IsExecuting = false;
    bool IsTerminating = false;

    struct
    {
        bool ItemSelected = false;
    } LastCommandPaletteStatus;

    struct
    {
        const char* NewSearchText = nullptr;
        bool FocusSearchBox = false;
    } NextCommandPaletteActions;

    void RegisterCommand(Command command)
    {
        auto location = std::lower_bound(
            Commands.begin(),
            Commands.end(),
            command,
            [](const Command& a, const Command& b) -> bool {
                return ImStricmp(a.Name.c_str(), b.Name.c_str()) < 0;
            });
        Commands.insert(location, std::move(command));
    }

    bool UnregisterCommand(const char* name)
    {
        struct Comparator
        {
            bool operator()(const Command& command, const char* str) const
            {
                return ImStricmp(command.Name.c_str(), str) < 0;
            }

            bool operator()(const char* str, const Command& command) const
            {
                return ImStricmp(str, command.Name.c_str()) < 0;
            }
        };

        auto range = std::equal_range(Commands.begin(), Commands.end(), name, Comparator{});
        Commands.erase(range.first, range.second);

        return range.first != range.second;
    }

    bool CommitOps()
    {
        if (IsCommandStorageLocked()) {
            return false;
        }

        for (auto& operation : PendingOps) {
            switch (operation.Type) {
                case CommandOperation::OpType_Register: {
                    auto& op = PendingRegisterOps[operation.Index];
                    RegisterCommand(std::move(op.Candidate));
                } break;

                case CommandOperation::OpType_Unregister: {
                    auto& op = PendingUnregisterOps[operation.Index];
                    UnregisterCommand(op.Name);
                } break;
            }
        }

        bool had_action = !PendingOps.empty();
        PendingRegisterOps.clear();
        PendingUnregisterOps.clear();
        PendingOps.clear();

        return had_action;
    }

    bool IsCommandStorageLocked() const
    {
        return CommandStorageLocks > 0;
    }
};

struct ItemExtraData
{
    bool Hovered = false;
    bool Held = false;
};

struct Instance
{
    ExecutionManager Session;
    SearchManager Search;
    std::vector<ItemExtraData> ExtraData;

    int CurrentSelectedItem = 0;

    struct
    {
        bool RefreshSearch = false;
        bool ClearSearch = false;
    } PendingActions;

    Instance()
        : Session(*this)
        , Search(*this) {}
};

static Context* gContext = nullptr;

// =================================================================
// Private implementation
// =================================================================

int ExecutionManager::GetItemCount() const
{
    if (m_ExecutingCommand) {
        return static_cast<int>(m_CallStack.back().Options.size());
    } else {
        return static_cast<int>(gContext->Commands.size());
    }
}

const char* ExecutionManager::GetItem(int idx) const
{
    if (m_ExecutingCommand) {
        return m_CallStack.back().Options[idx].c_str();
    } else {
        return gContext->Commands[idx].Name.c_str();
    }
}

template <class TFunc, class... Ts>
static void InvokeSafe(const TFunc& func, Ts&&... args)
{
    if (func) {
        func(std::forward<Ts>(args)...);
    }
}

void ExecutionManager::SelectItem(int idx)
{
    auto cmd = m_ExecutingCommand;
    size_t initial_call_stack_height = m_CallStack.size();

    // Guarding aginst invalid index.
    if (idx >= gContext->Commands.size()) return;
    IM_ASSERT(idx < gContext->Commands.size());

    if (cmd == nullptr) {
        cmd = m_ExecutingCommand = &gContext->Commands[idx];
        ++gContext->CommandStorageLocks;

        gContext->IsExecuting = true;
        InvokeSafe(m_ExecutingCommand->InitialCallback); // Calls ::Prompt()
        gContext->IsExecuting = false;
    } else {
        m_CallStack.back().SelectedOption = idx;

        gContext->IsExecuting = true;
        InvokeSafe(cmd->SubsequentCallback, idx); // Calls ::Prompt()
        gContext->IsExecuting = false;
    }

    size_t final_call_stack_height = m_CallStack.size();
    if (initial_call_stack_height == final_call_stack_height) {

        gContext->IsTerminating = true;
        InvokeSafe(m_ExecutingCommand->TerminatingCallback); // Shouldn't call ::Prompt()
        gContext->IsTerminating = false;

        m_ExecutingCommand = nullptr;
        m_CallStack.clear();
        --gContext->CommandStorageLocks;

        // If the executed command involved subcommands...
        if (final_call_stack_height > 0) {
            m_Instance->PendingActions.ClearSearch = true;
            m_Instance->CurrentSelectedItem = 0;
        }

        gContext->LastCommandPaletteStatus.ItemSelected = true;
    } else {
        // Something new is prompted
        // It doesn't make sense for "current selected item" to persists through completely different set of options
        m_Instance->PendingActions.ClearSearch = true;
        m_Instance->CurrentSelectedItem = 0;
    }
}

void ExecutionManager::PushOptions(std::vector<std::string> options)
{
    m_CallStack.push_back({});
    auto& frame = m_CallStack.back();

    frame.Options = std::move(options);

    m_Instance->PendingActions.ClearSearch = true;
}

int SearchManager::GetItemCount() const
{
    return static_cast<int>(SearchResults.size());
}

const char* SearchManager::GetItem(int idx) const
{
    int actualIdx = SearchResults[idx].ItemIndex;
    return m_Instance->Session.GetItem(actualIdx);
}

bool SearchManager::IsActive() const
{
    return SearchText[0] != '\0';
}

void SearchManager::SetSearchText(const char* text)
{
    // Copy at most IM_ARRAYSIZE(SearchText) chars from `text` to `SearchText`
    ImStrncpy(SearchText, text, IM_ARRAYSIZE(SearchText));
    RefreshSearchResults();
}

void SearchManager::ClearSearchText()
{
    // ImGui doesn't have a ImMemset either, they use std::memset too
    std::memset(SearchText, 0, IM_ARRAYSIZE(SearchText));
    SearchResults.clear();
}

void SearchManager::RefreshSearchResults()
{
    m_Instance->CurrentSelectedItem = 0;
    SearchResults.clear();

    int item_count = m_Instance->Session.GetItemCount();
    for (int i = 0; i < item_count; ++i) {
        const char* text = m_Instance->Session.GetItem(i);
        SearchResult result;
        if (FuzzySearch(SearchText, text, result.Score, result.Matches, IM_ARRAYSIZE(result.Matches), result.MatchCount)) {
            result.ItemIndex = i;
            SearchResults.push_back(result);
        }
    }

    std::sort(
        SearchResults.begin(),
        SearchResults.end(),
        [](const SearchResult& a, const SearchResult& b) -> bool {
            // We want the biggest element first
            return a.Score > b.Score;
        });
}

// =================================================================
// API implementation
// =================================================================

Context* CreateContext()
{
    auto ctx = new Context();
    if (!gContext) {
        gContext = ctx;
    }
    return ctx;
}

void DestroyContext()
{
    DestroyContext(gContext);
    gContext = nullptr;
}

void DestroyContext(Context* context)
{
    delete context;
}

void SetCurrentContext(Context* context)
{
    gContext = context;
}

Context* GetCurrentContext()
{
    return gContext;
}

void AddCommand(Command command)
{
    IM_ASSERT(gContext != nullptr);

    if (gContext->IsCommandStorageLocked()) {
        gContext->PendingRegisterOps.push_back(CommandOperationRegister{ std::move(command) });
        CommandOperation op;
        op.Type = CommandOperation::OpType_Register;
        op.Index = static_cast<int>(gContext->PendingRegisterOps.size()) - 1;
        gContext->PendingOps.push_back(op);
    } else {
        gContext->RegisterCommand(std::move(command));
    }

    if (auto current = gContext->CurrentCommandPalette) {
        current->PendingActions.RefreshSearch = true;
    }
}

void RemoveCommand(const char* name)
{
    IM_ASSERT(gContext != nullptr);

    if (gContext->IsCommandStorageLocked()) {
        gContext->PendingUnregisterOps.push_back(CommandOperationUnregister{ name });
        CommandOperation op;
        op.Type = CommandOperation::OpType_Unregister;
        op.Index = static_cast<int>(gContext->PendingUnregisterOps.size()) - 1;
        gContext->PendingOps.push_back(op);
    } else {
        gContext->UnregisterCommand(name);
    }

    if (auto current = gContext->CurrentCommandPalette) {
        current->PendingActions.RefreshSearch = true;
    }
}

bool GetStyleFlag(ImCmdTextType type, ImCmdTextFlag flag)
{
    IM_ASSERT(gContext != nullptr);
    return gContext->TextStyleFlags[type] & (1 << flag);
}

void SetStyleFlag(ImCmdTextType type, ImCmdTextFlag flag, bool enabled)
{
    IM_ASSERT(gContext != nullptr);
    if (enabled) {
        gContext->TextStyleFlags[type] |= 1 << flag;
    } else {
        gContext->TextStyleFlags[type] &= ~(1 << flag);
    }
}

ImFont* GetStyleFont(ImCmdTextType type)
{
    IM_ASSERT(gContext != nullptr);
    return gContext->TextStyleFonts[type];
}

void SetStyleFont(ImCmdTextType type, ImFont* font)
{
    IM_ASSERT(gContext != nullptr);
    gContext->TextStyleFonts[type] = font;
}

ImU32 GetStyleColor(ImCmdTextType type)
{
    IM_ASSERT(gContext != nullptr);
    return gContext->TextStyleColors[type];
}

void SetStyleColor(ImCmdTextType type, ImU32 color)
{
    IM_ASSERT(gContext != nullptr);
    gContext->TextStyleColors[type] = color;
    gContext->TextStyleHasColorOverride[type] = true;
}

void ClearStyleColor(ImCmdTextType type)
{
    IM_ASSERT(gContext != nullptr);
    gContext->TextStyleHasColorOverride[type] = false;
}

void SetNextCommandPaletteSearch(const char* text)
{
    IM_ASSERT(gContext != nullptr);
    IM_ASSERT(text != nullptr);
    gContext->NextCommandPaletteActions.NewSearchText = text;
}

void SetNextCommandPaletteSearchBoxFocused()
{
    IM_ASSERT(gContext != nullptr);
    gContext->NextCommandPaletteActions.FocusSearchBox = true;
}

void CommandPalette(const char* name)
{
    IM_ASSERT(gContext != nullptr);

    auto& gg = *gContext;
    auto& gi = *[&]() {
        auto id = ImHashStr(name);
        if (auto ptr = gg.Instances.GetVoidPtr(id)) {
            return reinterpret_cast<Instance*>(ptr);
        } else {
            auto instance = new Instance();
            gg.Instances.SetVoidPtr(id, instance);
            return instance;
        }
    }();

    float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    float search_result_window_height = 400.0f; // TODO config

    // BEGIN this command palette
    gg.CurrentCommandPalette = &gi;
    ImGui::PushID(name);

    gg.LastCommandPaletteStatus = {};

    // BEGIN processing PendingActions
    bool refresh_search = gi.PendingActions.RefreshSearch;
    refresh_search |= gg.CommitOps();

    if (auto text = gg.NextCommandPaletteActions.NewSearchText) {
        refresh_search = false;
        if (text[0] == '\0') {
            gi.Search.ClearSearchText();
        } else {
            gi.Search.SetSearchText(text);
        }
    } else if (gi.PendingActions.ClearSearch) {
        refresh_search = false;
        gi.Search.ClearSearchText();
    }

    if (refresh_search) {
        gi.Search.RefreshSearchResults();
    }

    gi.PendingActions = {};
    // END procesisng PendingActions

    if (gg.NextCommandPaletteActions.FocusSearchBox) {
        // Focus the search box when user first brings command palette window up
        // Note: this only affects the next frame
        ImGui::SetKeyboardFocusHere(0);
    }
    ImGui::SetNextItemWidth(width);
    if (ImGui::InputText("##SearchBox", gi.Search.SearchText, IM_ARRAYSIZE(gi.Search.SearchText))) {
        // Search string updated, update search results
        gi.Search.RefreshSearchResults();
    }

    ImGui::BeginChild("SearchResults", ImVec2(width, search_result_window_height));

    auto window = ImGui::GetCurrentContext()->CurrentWindow;
    auto draw_list = window->DrawList;

    auto font_regular = gg.TextStyleFonts[ImCmdTextType_Regular];
    if (!font_regular) {
        font_regular = ImGui::GetDrawListSharedData()->Font;
    }
    auto font_highlight = gg.TextStyleFonts[ImCmdTextType_Highlight];
    if (!font_highlight) {
        font_highlight = ImGui::GetDrawListSharedData()->Font;
    }

    ImU32 text_color_regular;
    ImU32 text_color_highlight;
    if (gg.TextStyleHasColorOverride[ImCmdTextType_Regular]) {
        text_color_regular = gg.TextStyleColors[ImCmdTextType_Regular];
    } else {
        text_color_regular = ImGui::GetColorU32(ImGuiCol_Text);
    }
    if (gg.TextStyleHasColorOverride[ImCmdTextType_Highlight]) {
        text_color_highlight = gg.TextStyleColors[ImCmdTextType_Highlight];
    } else {
        text_color_highlight = ImGui::GetColorU32(ImGuiCol_Text);
    }

    bool underline_regular = gg.TextStyleFlags[ImCmdTextType_Regular] & (1 << ImCmdTextFlag_Underline);
    bool underline_highlight = gg.TextStyleFlags[ImCmdTextType_Highlight] & (1 << ImCmdTextFlag_Underline);

    auto item_hovered_color = ImGui::GetColorU32(ImGuiCol_HeaderHovered);
    auto item_active_color = ImGui::GetColorU32(ImGuiCol_HeaderActive);
    auto item_selected_color = ImGui::GetColorU32(ImGuiCol_Header);

    // Could be 0.5 on macOS Retina, 1 elsewhere
    float font_scale = ImGui::GetIO().FontGlobalScale;

    int item_count;
    if (gi.Search.IsActive()) {
        item_count = gi.Search.GetItemCount();
    } else {
        item_count = gi.Session.GetItemCount();
    }

    if (gi.ExtraData.size() < item_count) {
        gi.ExtraData.resize(item_count);
    }

    // Flag used to delay item selection until after the loop ends
    bool select_focused_item = false;
    for (int i = 0; i < item_count; ++i) {
        // Implement a custom button-like control

        // We are doing this so that it can be highlighted without losing focus on the ImGui::InputText,
        // allowing the user to naviage with up/down arrow keys while typing.

        auto id = window->GetID(static_cast<int>(i));

        const float font_regular_FontSize = 12, font_highlight_FontSize = 14; //< @r-lyeh

        ImVec2 size{
            ImGui::GetContentRegionAvail().x,
            ImMax(font_regular_FontSize, font_highlight_FontSize),
        };
        ImRect rect{
            window->DC.CursorPos,
            window->DC.CursorPos + ImGui::CalcItemSize(size, 0.0f, 0.0f),
        };

        bool& hovered = gi.ExtraData[i].Hovered;
        bool& held = gi.ExtraData[i].Held;
        if (held && hovered) {
            draw_list->AddRectFilled(rect.Min, rect.Max, item_active_color);
        } else if (hovered) {
            draw_list->AddRectFilled(rect.Min, rect.Max, item_hovered_color);
        } else if (gi.CurrentSelectedItem == i) {
            draw_list->AddRectFilled(rect.Min, rect.Max, item_selected_color);
        }

        if (gi.Search.IsActive()) {
            // Iterating search results: draw text with highlights at matched chars

            auto& search_result = gi.Search.SearchResults[i];
            auto text = gi.Search.GetItem(i);

            auto text_pos = window->DC.CursorPos;
            int range_begin;
            int range_end;
            int last_range_end = 0;

            auto DrawCurrentRange = [&]() {
                if (range_begin != last_range_end) {
                    // Draw normal text between last highlighted range end and current highlighted range start
                    auto begin = text + last_range_end;
                    auto end = text + range_begin;

#if 0
                    draw_list->AddText(text_pos, text_color_regular, begin, end);
#else //< @r-lyeh
                    draw_list->AddText(font_regular, font_regular_FontSize * font_scale, text_pos, text_color_regular, begin, end);
#endif
                    auto segment_size = font_regular->CalcTextSizeA(font_regular_FontSize, (std::numeric_limits<float>::max)(), 0.0f, begin, end);

                    if (underline_regular) {
                        float x1 = text_pos.x;
                        float x2 = text_pos.x + segment_size.x;
                        float y = text_pos.y + segment_size.y;
                        // TODO adjust this to be at text baseline instead
                        draw_list->AddLine(ImVec2(x1, y), ImVec2(x2, y), text_color_regular);
                    }

                    text_pos.x += segment_size.x;
                }

                auto begin = text + range_begin;
                auto end = text + range_end;

                draw_list->AddText(font_highlight, font_highlight_FontSize * font_scale, text_pos, text_color_highlight, begin, end);
                auto segment_size = font_highlight->CalcTextSizeA(font_highlight_FontSize * font_scale, (std::numeric_limits<float>::max)(), 0.0f, begin, end);

                if (underline_highlight) {
                    float x1 = text_pos.x;
                    float x2 = text_pos.x + segment_size.x;
                    float y = text_pos.y + segment_size.y;
                    // TODO adjust this to be at text baseline instead
                    draw_list->AddLine(ImVec2(x1, y), ImVec2(x2, y), text_color_highlight);
                }

                text_pos.x += segment_size.x;
            };

            IM_ASSERT(search_result.MatchCount >= 1);
            range_begin = search_result.Matches[0];
            range_end = range_begin;

            int last_char_idx = -1;
            for (int j = 0; j < search_result.MatchCount; ++j) {
                int char_idx = search_result.Matches[j];

                if (char_idx == last_char_idx + 1) {
                    // These 2 indices are equal, extend our current range by 1
                    ++range_end;
                } else {
                    DrawCurrentRange();
                    last_range_end = range_end;
                    range_begin = char_idx;
                    range_end = char_idx + 1;
                }

                last_char_idx = char_idx;
            }

            // Draw the remaining range (if any)
            if (range_begin != range_end) {
                DrawCurrentRange();
            }

            // Draw the text after the last range (if any)
#if 0
            draw_list->AddText(text_pos, text_color_regular, text + range_end); // Draw until \0
#else //< @r-lyeh
            draw_list->AddText(font_regular, font_regular_FontSize * font_scale, text_pos, text_color_regular, text + range_end); // Draw until \0
#endif

        } else {
            // Iterating everything else: draw text as-is, there is no highlights

            auto text = gi.Session.GetItem(i);
            auto text_pos = window->DC.CursorPos;
#if 0
            draw_list->AddText(text_pos, text_color_regular, text);
#else //< @r-lyeh
            draw_list->AddText(font_regular, font_regular_FontSize * font_scale, text_pos, text_color_regular, text);
#endif
        }

        ImGui::ItemSize(rect);
        if (!ImGui::ItemAdd(rect, id)) {
            continue;
        }
        if (ImGui::ButtonBehavior(rect, id, &hovered, &held)) {
            gi.CurrentSelectedItem = i;
            select_focused_item = true;
        }
    }

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {
        gi.CurrentSelectedItem = ImMax(gi.CurrentSelectedItem - 1, 0);
    } else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
        gi.CurrentSelectedItem = ImMin(gi.CurrentSelectedItem + 1, item_count - 1);
    }
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) || select_focused_item) {
        if (gi.Search.IsActive() && !gi.Search.SearchResults.empty()) {
            auto idx = gi.Search.SearchResults[gi.CurrentSelectedItem].ItemIndex;
            gi.Session.SelectItem(idx);
        } else {
            gi.Session.SelectItem(gi.CurrentSelectedItem);
        }
    }

    ImGui::EndChild();

    gg.NextCommandPaletteActions = {};

    ImGui::PopID();
    gg.CurrentCommandPalette = nullptr;
    // END this command palette
}

bool IsAnyItemSelected()
{
    IM_ASSERT(gContext != nullptr);
    return gContext->LastCommandPaletteStatus.ItemSelected;
}

void RemoveCache(const char* name)
{
    IM_ASSERT(gContext != nullptr);

    auto& instances = gContext->Instances;
    auto id = ImHashStr(name);
    if (auto ptr = instances.GetVoidPtr(id)) {
        auto instance = reinterpret_cast<Instance*>(ptr);
        instances.SetVoidPtr(id, nullptr);
        delete instance;
    }
}

void RemoveAllCaches()
{
    IM_ASSERT(gContext != nullptr);

    auto& instances = gContext->Instances;
    for (auto& entry : instances.Data) {
        auto instance = reinterpret_cast<Instance*>(entry.val_p);
        entry.val_p = nullptr;
        delete instance;
    }
    instances = {};
}

void SetNextWindowAffixedTop(ImGuiCond cond)
{
#if 0
    auto viewport = ImGui::GetMainViewport()->Size;

    // Center window horizontally, align top vertically
    ImGui::SetNextWindowPos(ImVec2(viewport.x / 2, 0), cond, ImVec2(0.5f, 0.0f));
#else //< @r-lyeh
    // Center window horizontally, align top vertically
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->WorkPos.x + ImGui::GetMainViewport()->Size.x / 2, ImGui::GetMainViewport()->WorkPos.y), cond, ImVec2(0.5f, 0.0f));
#endif
}

void CommandPaletteWindow(const char* name, bool* p_open)
{
    auto viewport = ImGui::GetMainViewport()->Size;

    SetNextWindowAffixedTop();
    ImGui::SetNextWindowSize(ImVec2(viewport.x * 0.3f, 0.0f));
    ImGui::Begin(name, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::IsWindowAppearing()) {
        SetNextCommandPaletteSearchBoxFocused();
    }

    CommandPalette(name);

    if (IsAnyItemSelected()) {
        *p_open = false;
    }
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
        // Close popup when user unfocused the command palette window (clicking elsewhere)
        *p_open = false;
    }

    ImGui::End();
}

void Prompt(std::vector<std::string> options)
{
    IM_ASSERT(gContext != nullptr);
    IM_ASSERT(gContext->CurrentCommandPalette != nullptr);
    IM_ASSERT(gContext->IsExecuting);
    IM_ASSERT(!gContext->IsTerminating);

    auto& gi = *gContext->CurrentCommandPalette;
    gi.Session.PushOptions(std::move(options));
}
} // namespace ImCmd
