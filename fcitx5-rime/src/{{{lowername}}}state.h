/*
 * SPDX-FileCopyrightText: 2017~2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef _FCITX_{{{uppername}}}STATE_H_
#define _FCITX_{{{uppername}}}STATE_H_

#include "{{{lowername}}}engine.h"
#include <fcitx/inputcontextproperty.h>

namespace fcitx {

class {{{name}}}State : public InputContextProperty {
public:
    {{{name}}}State({{{name}}}Engine *engine, InputContext &ic);

    virtual ~{{{name}}}State();

    void clear();
    void keyEvent(KeyEvent &event);
    bool getStatus(const std::function<void(const {{{name}}}Status &)> &);
    void updatePreedit(InputContext *ic, const {{{name}}}Context &context);
    void updateUI(InputContext *ic, bool keyRelease);
    void release();
    void commitPreedit(InputContext *ic);
    std::string subMode();
    std::string subModeLabel();
    void setLatinMode(bool latin);
    void selectSchema(const std::string &schemaId);

private:
    void createSession();

    std::string lastMode_;
    {{{name}}}Engine *engine_;
    {{{name}}}SessionId session_ = 0;
    InputContext &ic_;
};
} // namespace fcitx

#endif // _FCITX_{{{uppername}}}STATE_H_
