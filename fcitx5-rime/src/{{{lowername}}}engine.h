/*
 * SPDX-FileCopyrightText: 2017~2017 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef _FCITX_{{{uppername}}}ENGINE_H_
#define _FCITX_{{{uppername}}}ENGINE_H_

#include "{{{lowername}}}service.h"
#include <fcitx-config/configuration.h>
#include <fcitx-config/iniparser.h>
#include <fcitx-utils/event.h>
#include <fcitx-utils/eventdispatcher.h>
#include <fcitx-utils/i18n.h>
#include <fcitx-utils/library.h>
#include <fcitx-utils/log.h>
#include <fcitx/action.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addonmanager.h>
#include <fcitx/icontheme.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>
#include <fcitx/menu.h>
#include <memory>
#include <{{{lowername}}}_api.h>

namespace fcitx {

class {{{name}}}State;

FCITX_CONFIGURATION(
    {{{name}}}EngineConfig,
    Option<bool> showPreeditInApplication{this, "PreeditInApplication",
                                          _("Show preedit within application"),
                                          true};
    Option<bool> preeditCursorPositionAtBeginning{
        this, "PreeditCursorPositionAtBeginning",
        _("Fix embedded preedit cursor at the beginning of the preedit"), true};
    Option<bool> commitWhenDeactivate{
        this, "Commit when deactivate",
        _("Commit current text when deactivating"), true};
    ExternalOption userDataDir{
        this, "UserDataDir", _("User data dir"),
        stringutils::concat(
            "xdg-open \"",
            stringutils::replaceAll(
                stringutils::joinPath(StandardPath::global().userDirectory(
                                          StandardPath::Type::PkgData),
                                      "{{{lowername}}}"),
                "\"", "\"\"\""),
            "\"")};
#ifdef FCITX_{{{uppername}}}_LOAD_PLUGIN
    Option<bool> autoloadPlugins{this, "AutoloadPlugins",
                                 _("Load available plugins automatically"),
                                 false};
    Option<std::vector<std::string>> plugins{this, "Plugins", _("Plugins"),
                                             std::vector<std::string>()};
    Option<std::vector<std::string>> modules{this, "Modules", _("Modules"),
                                             std::vector<std::string>()};
#endif
);

class {{{name}}}Engine final : public InputMethodEngineV2 {
public:
    {{{name}}}Engine(Instance *instance);
    ~{{{name}}}Engine();
    Instance *instance() { return instance_; }
    void activate(const InputMethodEntry &entry,
                  InputContextEvent &event) override;
    void deactivate(const InputMethodEntry &entry,
                    InputContextEvent &event) override;
    void keyEvent(const InputMethodEntry &entry, KeyEvent &keyEvent) override;
    void reloadConfig() override;
    void reset(const InputMethodEntry &entry,
               InputContextEvent &event) override;
    void save() override;
    auto &factory() { return factory_; }

    void updateAction(InputContext *inputContext) {
        imAction_->update(inputContext);
    }

    const Configuration *getConfig() const override { return &config_; }
    void setConfig(const RawConfig &config) override {
        config_.load(config, true);
        safeSaveAsIni(config_, "conf/{{{lowername}}}.conf");
        updateConfig();
    }
    void setSubConfig(const std::string &path, const RawConfig &) override;
    void updateConfig();

    std::string subMode(const InputMethodEntry &, InputContext &) override;
    std::string subModeIconImpl(const InputMethodEntry &,
                                InputContext &) override;
    std::string subModeLabelImpl(const InputMethodEntry &,
                                 InputContext &) override;
    const {{{name}}}EngineConfig &config() const { return config_; }

    {{{lowername}}}_api_t *api() { return api_; }
    const auto &appOptions() const { return appOptions_; }

    void {{{lowername}}}Start(bool fullcheck);

    {{{name}}}State *state(InputContext *ic);

    FCITX_ADDON_DEPENDENCY_LOADER(dbus, instance_->addonManager());

private:
    static void {{{lowername}}}NotificationHandler(void *context_object,
                                        {{{name}}}SessionId session_id,
                                        const char *message_type,
                                        const char *message_value);

    void deploy();
    void sync();
    void updateSchemaMenu();
    void notify(const std::string &type, const std::string &value);

    IconTheme theme_;
    Instance *instance_;
    EventDispatcher eventDispatcher_;
    {{{lowername}}}_api_t *api_;
    bool firstRun_ = true;
    FactoryFor<{{{name}}}State> factory_;

    std::unique_ptr<Action> imAction_;
    SimpleAction deployAction_;
    SimpleAction syncAction_;

    {{{name}}}EngineConfig config_;
    std::unordered_map<std::string, std::unordered_map<std::string, bool>>
        appOptions_;

    FCITX_ADDON_DEPENDENCY_LOADER(notifications, instance_->addonManager());

    std::list<SimpleAction> schemActions_;
    Menu schemaMenu_;
#ifdef FCITX_{{{uppername}}}_LOAD_PLUGIN
    std::unordered_map<std::string, Library> pluginPool_;
#endif
    std::unique_ptr<EventSourceTime> timeEvent_;

    {{{name}}}Service service_{this};
};

class {{{name}}}EngineFactory : public AddonFactory {
public:
    AddonInstance *create(AddonManager *manager) override {
        registerDomain("fcitx5-{{{lowername}}}", FCITX_INSTALL_LOCALEDIR);
        return new {{{name}}}Engine(manager->instance());
    }
};
} // namespace fcitx

FCITX_DECLARE_LOG_CATEGORY({{{lowername}}});

#define {{{uppername}}}_DEBUG() FCITX_LOGC({{{lowername}}}, Debug)
#define {{{uppername}}}_ERROR() FCITX_LOGC({{{lowername}}}, Error)

#endif // _FCITX_{{{uppername}}}ENGINE_H_
