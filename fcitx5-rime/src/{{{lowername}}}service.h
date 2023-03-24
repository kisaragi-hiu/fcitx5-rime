/*
 * SPDX-FileCopyrightText: 2021~2021 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */
#ifndef _FCITX5_{{{uppername}}}_{{{uppername}}}SERVICE_H_
#define _FCITX5_{{{uppername}}}_{{{uppername}}}SERVICE_H_

#include <fcitx-utils/dbus/message.h>
#include <fcitx-utils/dbus/objectvtable.h>

namespace fcitx {

class {{{name}}}Engine;
class {{{name}}}State;

class {{{name}}}Service : public dbus::ObjectVTable<{{{name}}}Service> {
public:
    {{{name}}}Service({{{name}}}Engine *engine);

    void setAsciiMode(bool asciiMode);
    bool isAsciiMode();
    void setSchema(const std::string &schema);
    std::string currentSchema();
    std::vector<std::string> listAllSchemas();

private:
    {{{name}}}State *currentState();
    FCITX_OBJECT_VTABLE_METHOD(setAsciiMode, "SetAsciiMode", "b", "");
    FCITX_OBJECT_VTABLE_METHOD(isAsciiMode, "IsAsciiMode", "", "b");
    FCITX_OBJECT_VTABLE_METHOD(setSchema, "SetSchema", "s", "");
    FCITX_OBJECT_VTABLE_METHOD(currentSchema, "GetCurrentSchema", "", "s");
    FCITX_OBJECT_VTABLE_METHOD(listAllSchemas, "ListAllSchemas", "", "as");

    {{{name}}}Engine *engine_;
};

} // namespace fcitx

#endif // _FCITX5_{{{uppername}}}_{{{uppername}}}SERVICE_H_
