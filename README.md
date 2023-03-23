# Rebranded RIME

A hack to use be able to switch to a different RIME schema as an Fcitx5-level input method.

If you use RIME for just one language (typically Mandarin), you won't switch schemas that often, so having it be on a separate key is fine.

But if you use RIME for more languages, like Cantonese or Taiwanese, now switching languages requires you to think about which engine the input method uses — RIME, or Fcitx directly.

A proper fix for this would involve somehow Fcitx, IBus, or any other embedding engine to pick up RIME schemas, and also somehow distinguish between schemas for different languages. It's unlikely to ever happen.

Creating a separate build of fcitx5-rime is a way to work around this.

This separate build needs to have a different name. Not only that, if the same librime is used, fcitx5-rime and the separate build would end up sharing state: deployed schemas, current schema, and so on; so we also need to fork librime.
