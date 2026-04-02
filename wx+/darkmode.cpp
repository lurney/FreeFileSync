// *****************************************************************************
// * This file is part of the FreeFileSync project. It is distributed under    *
// * GNU General Public License: https://www.gnu.org/licenses/gpl-3.0          *
// * Copyright (C) Zenju (zenju AT freefilesync DOT org) - All Rights Reserved *
// *****************************************************************************

#include "darkmode.h"
#include <zen/sys_version.h>
#include <wx/settings.h>
#include "color_tools.h"
    #include <gtk/gtk.h>

using namespace zen;


std::optional<bool> globalDefaultThemeIsDark;


bool zen::darkModeAvailable()
{
    return true;

}


void zen::colorThemeInit(wxApp& app, ColorTheme colTheme) //throw FileError
{
    globalDefaultThemeIsDark = wxSystemSettings::GetAppearance().AreAppsDark();

    //caveat: on macOS there are more themes than light/dark: https://developer.apple.com/documentation/appkit/nsappearance/name-swift.struct
    if (colTheme != ColorTheme::System && //"System" is already the default for macOS/Linux(GTK3)
        darkModeAvailable())
        changeColorTheme(colTheme); //throw FileError
}


void zen::colorThemeCleanup()
{
    //nothing to do without color hook
}


bool zen::equalAppearance(ColorTheme colTheme1, ColorTheme colTheme2)
{
    if (colTheme1 == ColorTheme::System) colTheme1 = *globalDefaultThemeIsDark ? ColorTheme::Dark : ColorTheme::Light;
    if (colTheme2 == ColorTheme::System) colTheme2 = *globalDefaultThemeIsDark ? ColorTheme::Dark : ColorTheme::Light;
    return colTheme1 == colTheme2;
}


void zen::changeColorTheme(ColorTheme colTheme) //throw FileError
{
    if (colTheme == ColorTheme::System) //SetAppearance(System) isn't working reliably! surprise!?
        colTheme = *globalDefaultThemeIsDark ? ColorTheme::Dark : ColorTheme::Light;

    try
    {
        if (wxApp::AppearanceResult rv = wxTheApp->SetAppearance(colTheme);
            rv != wxApp::AppearanceResult::Ok)
            throw SysError(formatSystemError("wxApp::SetAppearance",
                                             rv == wxApp::AppearanceResult::CannotChange ? L"CannotChange" : L"Failure", L"" /*errorMsg*/));
    }
    catch (const SysError& e) { throw FileError(_("Failed to update the color theme."), e.toString()); }
}
