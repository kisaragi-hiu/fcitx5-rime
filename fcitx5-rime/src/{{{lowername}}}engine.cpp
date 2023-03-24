/*
 * SPDX-FileCopyrightText: 2017~2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "{{{lowername}}}engine.h"
#include "notifications_public.h"
#include "{{{lowername}}}state.h"
#include <cstring>
#include <dirent.h>
#include <fcitx-utils/event.h>
#include <fcitx-utils/fs.h>
#include <fcitx-utils/i18n.h>
#include <fcitx-utils/log.h>
#include <fcitx-utils/standardpath.h>
#include <fcitx/candidatelist.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputcontextmanager.h>
#include <fcitx/inputpanel.h>
#include <fcitx/userinterfacemanager.h>
#include <{{{lowername}}}_api.h>

FCITX_DEFINE_LOG_CATEGORY({{{lowername}}}, "{{{lowername}}}");

namespace fcitx {

namespace {

std::unordered_map<std::string, std::unordered_map<std::string, bool>>
parseAppOptions({{{lowername}}}_api_t *api, {{{name}}}Config *config) {
    std::unordered_map<std::string, std::unordered_map<std::string, bool>>
        appOptions;
    {{{name}}}ConfigIterator appIter;
    {{{name}}}ConfigIterator optionIter;
    api->config_begin_map(&appIter, config, "app_options");
    while (api->config_next(&appIter)) {
        auto &options = appOptions[appIter.key];
        api->config_begin_map(&optionIter, config, appIter.path);
        while (api->config_next(&optionIter)) {
            Bool value = False;
            if (api->config_get_bool(config, optionIter.path, &value)) {
                options[optionIter.key] = !!value;
            }
        }
        api->config_end(&optionIter);
    }
    api->config_end(&appIter);
    return appOptions;
}
} // namespace

class IMAction : public Action {
public:
    IMAction({{{name}}}Engine *engine) : engine_(engine) {}

    std::string shortText(InputContext *ic) const override {
        std::string result;
        auto state = engine_->state(ic);
        if (state) {
            state->getStatus([&result](const {{{name}}}Status &status) {
                result = status.schema_id ? status.schema_id : "";
                if (status.is_disabled) {
                    result = "\xe2\x8c\x9b";
                } else if (status.is_ascii_mode) {
                    result = "A";
                } else if (status.schema_name && status.schema_name[0] != '.') {
                    result = status.schema_name;
                } else {
                    result = "中";
                }
            });
        } else {
            result = "\xe2\x8c\x9b";
        }
        return result;
    }

    std::string longText(InputContext *ic) const override {
        std::string result;
        auto state = engine_->state(ic);
        if (state) {
            state->getStatus([&result](const {{{name}}}Status &status) {
                result = status.schema_name ? status.schema_name : "";
            });
        }
        return result;
    }

    std::string icon(InputContext *ic) const override {
        bool isDisabled = false;
        auto state = engine_->state(ic);
        if (state) {
            state->getStatus([&isDisabled](const {{{name}}}Status &status) {
                isDisabled = status.is_disabled;
            });
        }
        if (isDisabled) {
            return "fcitx-{{{lowername}}}-disabled";
        }
        return "fcitx-{{{lowername}}}-im";
    }

private:
    {{{name}}}Engine *engine_;
};

{{{name}}}Engine::{{{name}}}Engine(Instance *instance)
    : instance_(instance), api_({{{lowername}}}_get_api()),
      factory_([this](InputContext &ic) { return new {{{name}}}State(this, ic); }) {
    imAction_ = std::make_unique<IMAction>(this);
    instance_->userInterfaceManager().registerAction("fcitx-{{{lowername}}}-im",
                                                     imAction_.get());
    imAction_->setMenu(&schemaMenu_);
    eventDispatcher_.attach(&instance_->eventLoop());
    deployAction_.setIcon("fcitx-{{{lowername}}}-deploy");
    deployAction_.setShortText(_("Deploy"));
    deployAction_.connect<SimpleAction::Activated>([this](InputContext *ic) {
        deploy();
        auto state = this->state(ic);
        if (state && ic->hasFocus()) {
            state->updateUI(ic, false);
        }
    });
    instance_->userInterfaceManager().registerAction("fcitx-{{{lowername}}}-deploy",
                                                     &deployAction_);

    syncAction_.setIcon("fcitx-{{{lowername}}}-sync");
    syncAction_.setShortText(_("Synchronize"));

    syncAction_.connect<SimpleAction::Activated>([this](InputContext *ic) {
        sync();
        auto state = this->state(ic);
        if (state && ic->hasFocus()) {
            state->updateUI(ic, false);
        }
    });
    instance_->userInterfaceManager().registerAction("fcitx-{{{lowername}}}-sync",
                                                     &syncAction_);
    reloadConfig();
}

{{{name}}}Engine::~{{{name}}}Engine() {
    factory_.unregister();
    try {
        if (api_) {
            api_->finalize();
        }
    } catch (const std::exception &e) {
        {{{uppername}}}_ERROR() << e.what();
    }
}

void {{{name}}}Engine::{{{lowername}}}Start(bool fullcheck) {
    if (!api_) {
        return;
    }

    {{{uppername}}}_DEBUG() << "{{{name}}} Start (fullcheck: " << fullcheck << ")";

    auto userDir = stringutils::joinPath(
        StandardPath::global().userDirectory(StandardPath::Type::PkgData),
        "{{{lowername}}}");
    if (!fs::makePath(userDir)) {
        if (!fs::isdir(userDir)) {
            {{{uppername}}}_ERROR() << "Failed to create user directory: " << userDir;
        }
    }
    const char *sharedDataDir = {{{uppername}}}_DATA_DIR;

    {{{uppername}}}_STRUCT({{{name}}}Traits, fcitx_{{{lowername}}}_traits);
    fcitx_{{{lowername}}}_traits.shared_data_dir = sharedDataDir;
    // To stop {{{name}}} from initializing glog twice
    // fcitx_{{{lowername}}}_traits.app_name = "{{{lowername}}}.fcitx-{{{lowername}}}";
    fcitx_{{{lowername}}}_traits.user_data_dir = userDir.c_str();
    fcitx_{{{lowername}}}_traits.distribution_name = "{{{name}}}";
    fcitx_{{{lowername}}}_traits.distribution_code_name = "fcitx-{{{lowername}}}";
    fcitx_{{{lowername}}}_traits.distribution_version = FCITX_{{{uppername}}}_VERSION;

#ifdef FCITX_{{{uppername}}}_LOAD_PLUGIN
    std::vector<const char *> modules;
    // When it is not test, {{{lowername}}} will load the default set.
    {{{uppername}}}_DEBUG() << "Modules: " << *config_.modules;
    if (!config_.modules->empty()) {
        modules.push_back("default");
        for (const std::string &module : *config_.modules) {
            modules.push_back(module.data());
        }
        modules.push_back(nullptr);
        fcitx_{{{lowername}}}_traits.modules = modules.data();
    } else {
        fcitx_{{{lowername}}}_traits.modules = nullptr;
    }
#else
    fcitx_{{{lowername}}}_traits.modules = nullptr;
#endif

    if (firstRun_) {
        api_->setup(&fcitx_{{{lowername}}}_traits);
        firstRun_ = false;
    }
    api_->initialize(&fcitx_{{{lowername}}}_traits);
    api_->set_notification_handler(&{{{lowername}}}NotificationHandler, this);
    if (api_->start_maintenance(fullcheck)) {
        api_->deploy_config_file("fcitx5_{{{lowername}}}.yaml", "config_version");
    }

    appOptions_.clear();
    {{{name}}}Config config = {NULL};
    if (api_->config_open("fcitx5", &config)) {
        appOptions_ = parseAppOptions(api_, &config);
        api_->config_close(&config);
    }
    {{{uppername}}}_DEBUG() << "App options are " << appOptions_;
}

void {{{name}}}Engine::reloadConfig() {
    readAsIni(config_, "conf/{{{lowername}}}.conf");
    updateConfig();
}

void {{{name}}}Engine::setSubConfig(const std::string &path, const RawConfig &) {
    if (path == "deploy") {
        deploy();
    } else if (path == "sync") {
        sync();
    }
}

void {{{name}}}Engine::updateConfig() {
    {{{uppername}}}_DEBUG() << "{{{name}}} UpdateConfig";
    factory_.unregister();
    if (api_) {
        try {
            api_->finalize();
        } catch (const std::exception &e) {
            {{{uppername}}}_ERROR() << e.what();
        }
    }

#ifdef FCITX_{{{uppername}}}_LOAD_PLUGIN
    std::vector<std::string> plugins;
    if (*config_.autoloadPlugins) {
        auto closedir0 = [](DIR *dir) {
            if (dir) {
                closedir(dir);
            }
        };

        const char *libdir = StandardPath::fcitxPath("libdir");
        std::unique_ptr<DIR, void (*)(DIR *)> scopedDir{opendir(libdir),
                                                        closedir0};
        if (scopedDir) {
            auto dir = scopedDir.get();
            struct dirent *drt;
            while ((drt = readdir(dir)) != nullptr) {
                if (strcmp(drt->d_name, ".") == 0 ||
                    strcmp(drt->d_name, "..") == 0) {
                    continue;
                }

                auto name = drt->d_name;
                if (stringutils::startsWith(name, "lib{{{lowername}}}-") &&
                    stringutils::endsWith(name, ".so")) {
                    plugins.push_back(
                        stringutils::joinPath(libdir, std::move(name)));
                }
            }
        }
    } else {
        plugins = *config_.plugins;
    }

    for (const std::string &plugin : plugins) {
        if (pluginPool_.count(plugin)) {
            continue;
        }
        pluginPool_.emplace(plugin, Library(plugin));
        pluginPool_[plugin].load({LibraryLoadHint::ExportExternalSymbolsHint});
        {{{uppername}}}_DEBUG() << "Trying to load {{{lowername}}} plugin: " << plugin;
        if (!pluginPool_[plugin].loaded()) {
            {{{uppername}}}_ERROR() << "Failed to load plugin: " << plugin
                         << " error: " << pluginPool_[plugin].error();
            pluginPool_.erase(plugin);
        }
    }
#endif

    {{{lowername}}}Start(false);
    instance_->inputContextManager().registerProperty("{{{lowername}}}State",
                                                      &factory_);
    updateSchemaMenu();
}
void {{{name}}}Engine::activate(const InputMethodEntry &, InputContextEvent &event) {
    event.inputContext()->statusArea().addAction(StatusGroup::InputMethod,
                                                 imAction_.get());
    event.inputContext()->statusArea().addAction(StatusGroup::InputMethod,
                                                 &deployAction_);
    event.inputContext()->statusArea().addAction(StatusGroup::InputMethod,
                                                 &syncAction_);
}
void {{{name}}}Engine::deactivate(const InputMethodEntry &entry,
                            InputContextEvent &event) {
    if (event.type() == EventType::InputContextSwitchInputMethod &&
        *config_.commitWhenDeactivate) {
        auto inputContext = event.inputContext();
        auto state = inputContext->propertyFor(&factory_);
        state->commitPreedit(inputContext);
    }
    reset(entry, event);
}
void {{{name}}}Engine::keyEvent(const InputMethodEntry &entry, KeyEvent &event) {
    FCITX_UNUSED(entry);
    {{{uppername}}}_DEBUG() << "{{{name}}} receive key: " << event.rawKey() << " "
                 << event.isRelease();
    auto inputContext = event.inputContext();
    auto state = inputContext->propertyFor(&factory_);
    state->keyEvent(event);
}

void {{{name}}}Engine::reset(const InputMethodEntry &, InputContextEvent &event) {
    auto inputContext = event.inputContext();

    auto state = inputContext->propertyFor(&factory_);
    state->clear();
    inputContext->inputPanel().reset();
    inputContext->updatePreedit();
    inputContext->updateUserInterface(UserInterfaceComponent::InputPanel);
}

void {{{name}}}Engine::save() {}

void {{{name}}}Engine::{{{lowername}}}NotificationHandler(void *context, {{{name}}}SessionId session,
                                         const char *messageType,
                                         const char *messageValue) {
    {{{uppername}}}_DEBUG() << "Notification: " << session << " " << messageType << " "
                 << messageValue;
    {{{name}}}Engine *that = static_cast<{{{name}}}Engine *>(context);
    that->eventDispatcher_.schedule(
        [that, messageType = std::string(messageType),
         messageValue = std::string(messageValue)]() {
            that->notify(messageType, messageValue);
        });
}

void {{{name}}}Engine::notify(const std::string &messageType,
                        const std::string &messageValue) {
    const char *message = nullptr;
    const char *icon = "";
    const char *tipId = "";
    if (messageType == "deploy") {
        tipId = "fcitx-{{{lowername}}}-deploy";
        icon = "fcitx-{{{lowername}}}-deploy";
        if (messageValue == "start") {
            message = _("{{{name}}} is under maintenance. It may take a few "
                        "seconds. Please wait until it is finished...");
        } else if (messageValue == "success") {
            message = _("{{{name}}} is ready.");
            updateSchemaMenu();
        } else if (messageValue == "failure") {
            message = _("{{{name}}} has encountered an error. "
                        "See /tmp/{{{lowername}}}.fcitx.ERROR for details.");
        }
    } else if (messageType == "option") {
        icon = "fcitx-{{{lowername}}}";
        if (messageValue == "!full_shape") {
            tipId = "fcitx-{{{lowername}}}-full-shape";
            message = _("Half Shape is enabled.");
        } else if (messageValue == "full_shape") {
            tipId = "fcitx-{{{lowername}}}-full-shape";
            message = _("Full Shape is enabled.");
        } else if (messageValue == "!ascii_punct") {
            tipId = "fcitx-{{{lowername}}}-ascii-punct";
            message = _("Punctuation conversion is enabled.");
        } else if (messageValue == "ascii_punct") {
            tipId = "fcitx-{{{lowername}}}-ascii-punct";
            message = _("Punctuation conversion is disabled.");
        } else if (messageValue == "!simplification") {
            tipId = "fcitx-{{{lowername}}}-simplification";
            message = _("Traditional Chinese is enabled.");
        } else if (messageValue == "simplification") {
            tipId = "fcitx-{{{lowername}}}-simplification";
            message = _("Simplified Chinese is enabled.");
        }
    }

    auto notifications = this->notifications();
    if (message && notifications) {
        notifications->call<INotifications::showTip>(tipId, _("{{{name}}}"), icon,
                                                     _("{{{name}}}"), message, -1);
    }
    timeEvent_ = instance_->eventLoop().addTimeEvent(
        CLOCK_MONOTONIC, now(CLOCK_MONOTONIC) + 1000000, 0,
        [this](EventSourceTime *, uint64_t) {
            if (auto *ic = instance_->lastFocusedInputContext()) {
                imAction_->update(ic);
                ic->updateUserInterface(UserInterfaceComponent::StatusArea);
            }
            return true;
        });
}

{{{name}}}State *{{{name}}}Engine::state(InputContext *ic) {
    if (!factory_.registered()) {
        return nullptr;
    }
    return ic->propertyFor(&factory_);
}

std::string {{{name}}}Engine::subMode(const InputMethodEntry &, InputContext &ic) {
    if (auto {{{lowername}}}State = state(&ic)) {
        return {{{lowername}}}State->subMode();
    }
    return "";
}

std::string {{{name}}}Engine::subModeLabelImpl(const InputMethodEntry &,
                                         InputContext &ic) {
    if (auto {{{lowername}}}State = state(&ic)) {
        return {{{lowername}}}State->subModeLabel();
    }
    return "";
}

std::string {{{name}}}Engine::subModeIconImpl(const InputMethodEntry &,
                                        InputContext &ic) {
    std::string result = "fcitx-{{{lowername}}}";
    if (!api_ || !factory_.registered()) {
        return result;
    }
    auto state = this->state(&ic);
    if (state) {
        state->getStatus([&result](const {{{name}}}Status &status) {
            if (status.is_disabled) {
                result = "fcitx-{{{lowername}}}-disable";
            } else if (status.is_ascii_mode) {
                result = "fcitx-{{{lowername}}}-latin";
            } else {
                result = "fcitx-{{{lowername}}}";
            }
        });
    }
    return result;
}

void {{{name}}}Engine::deploy() {
    {{{uppername}}}_DEBUG() << "{{{name}}} Deploy";
    instance_->inputContextManager().foreach([this](InputContext *ic) {
        if (auto state = this->state(ic)) {
            state->release();
        }
        return true;
    });
    api_->finalize();
    {{{lowername}}}Start(true);
}

void {{{name}}}Engine::sync() { api_->sync_user_data(); }

void {{{name}}}Engine::updateSchemaMenu() {
    if (!api_) {
        return;
    }

    schemActions_.clear();
    {{{name}}}SchemaList list;
    list.size = 0;
    if (api_->get_schema_list(&list)) {
        schemActions_.emplace_back();

        schemActions_.back().setShortText(_("Latin Mode"));
        schemActions_.back().connect<SimpleAction::Activated>(
            [this](InputContext *ic) {
                auto state = ic->propertyFor(&factory_);
                state->setLatinMode(true);
                imAction_->update(ic);
            });
        instance_->userInterfaceManager().registerAction(&schemActions_.back());
        schemaMenu_.addAction(&schemActions_.back());
        for (size_t i = 0; i < list.size; i++) {
            schemActions_.emplace_back();
            std::string schemaId = list.list[i].schema_id;
            auto &schemaAction = schemActions_.back();
            schemaAction.setShortText(list.list[i].name);
            schemaAction.connect<SimpleAction::Activated>(
                [this, schemaId](InputContext *ic) {
                    auto state = ic->propertyFor(&factory_);
                    state->selectSchema(schemaId);
                    imAction_->update(ic);
                });
            instance_->userInterfaceManager().registerAction(&schemaAction);
            schemaMenu_.addAction(&schemaAction);
        }
        api_->free_schema_list(&list);
    }
}

} // namespace fcitx

FCITX_ADDON_FACTORY(fcitx::{{{name}}}EngineFactory)
