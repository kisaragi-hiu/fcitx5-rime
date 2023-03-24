/*
 * SPDX-FileCopyrightText: 2017~2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef _FCITX_{{{uppername}}}CANDIDATE_H_
#define _FCITX_{{{uppername}}}CANDIDATE_H_

#include "{{{lowername}}}engine.h"
#include "{{{lowername}}}state.h"
#include <fcitx/candidatelist.h>

namespace fcitx {

class {{{name}}}CandidateWord : public CandidateWord {
public:
    {{{name}}}CandidateWord({{{name}}}Engine *engine, const {{{name}}}Candidate &candidate,
                      KeySym sym);

    void select(InputContext *inputContext) const override;

private:
    {{{name}}}Engine *engine_;
    KeySym sym_;
};

class {{{name}}}CandidateList final : public CandidateList,
                                public PageableCandidateList {
public:
    {{{name}}}CandidateList({{{name}}}Engine *engine, InputContext *ic,
                      const {{{name}}}Context &context);

    const Text &label(int idx) const override {
        checkIndex(idx);
        return labels_[idx];
    }

    const CandidateWord &candidate(int idx) const override {
        checkIndex(idx);
        return *candidateWords_[idx];
    }
    int size() const override { return candidateWords_.size(); }

    int cursorIndex() const override { return cursor_; }

    CandidateLayoutHint layoutHint() const override { return layout_; }

    bool hasPrev() const override { return hasPrev_; }
    bool hasNext() const override { return hasNext_; }
    void prev() override {
        KeyEvent event(ic_, Key(FcitxKey_Page_Up));
        if (auto state = engine_->state(ic_)) {
            state->keyEvent(event);
        }
    }
    void next() override {
        KeyEvent event(ic_, Key(FcitxKey_Page_Down));
        if (auto state = engine_->state(ic_)) {
            state->keyEvent(event);
        }
    }

    bool usedNextBefore() const override { return true; }

private:
    void checkIndex(int idx) const {
        if (idx < 0 && idx >= size()) {
            throw std::invalid_argument("invalid index");
        }
    }

    {{{name}}}Engine *engine_;
    InputContext *ic_;
    std::vector<Text> labels_;
    bool hasPrev_ = false;
    bool hasNext_ = false;
    CandidateLayoutHint layout_ = CandidateLayoutHint::NotSet;
    int cursor_ = -1;
    std::vector<std::unique_ptr<CandidateWord>> candidateWords_;
};
} // namespace fcitx

#endif // _FCITX_{{{uppername}}}CANDIDATE_H_
