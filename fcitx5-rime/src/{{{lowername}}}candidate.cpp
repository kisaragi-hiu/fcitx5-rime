/*
 * SPDX-FileCopyrightText: 2017~2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "{{{lowername}}}candidate.h"
#include <cstring>

namespace fcitx {

{{{name}}}CandidateWord::{{{name}}}CandidateWord({{{name}}}Engine *engine,
                                     const {{{name}}}Candidate &candidate, KeySym sym)
    : CandidateWord(), engine_(engine), sym_(sym) {
    Text text;
    text.append(std::string(candidate.text));
    if (candidate.comment && strlen(candidate.comment)) {
        text.append(" ");
        text.append(std::string(candidate.comment));
    }
    setText(text);
}

void {{{name}}}CandidateWord::select(InputContext *inputContext) const {
    // {{{name}}} does not provide such an API, simulate the selection with a fake
    // key event.
    if (auto state = engine_->state(inputContext)) {
        KeyEvent event(inputContext, Key(sym_));
        state->keyEvent(event);
    }
}

{{{name}}}CandidateList::{{{name}}}CandidateList({{{name}}}Engine *engine, InputContext *ic,
                                     const {{{name}}}Context &context)
    : engine_(engine), ic_(ic), hasPrev_(context.menu.page_no != 0),
      hasNext_(!context.menu.is_last_page) {
    setPageable(this);

    const auto &menu = context.menu;

    int num_select_keys = menu.select_keys ? strlen(menu.select_keys) : 0;
    bool has_label = {{{uppername}}}_STRUCT_HAS_MEMBER(context, context.select_labels) &&
                     context.select_labels;

    int i;
    for (i = 0; i < menu.num_candidates; ++i) {
        KeySym sym = FcitxKey_None;
        std::string label;
        if (i < menu.page_size && has_label) {
            label = context.select_labels[i];
        } else if (i < num_select_keys) {
            label = std::string(1, menu.select_keys[i]);
        } else {
            label = std::to_string((i + 1) % 10);
        }
        label.append(" ");
        labels_.emplace_back(label);

        if (i < num_select_keys) {
            sym = static_cast<KeySym>(menu.select_keys[i]);
        } else {
            sym = static_cast<KeySym>('0' + (i + 1) % 10);
        }
        candidateWords_.emplace_back(std::make_unique<{{{name}}}CandidateWord>(
            engine, menu.candidates[i], sym));

        if (i == menu.highlighted_candidate_index) {
            cursor_ = i;
        }
    }
}
} // namespace fcitx
