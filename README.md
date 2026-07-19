## RIME support for Fcitx

RIME(中州韻輸入法引擎) is _mainly_ a Traditional Chinese input method engine.

[![Jenkins Build](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins.fcitx-im.org%2Fjob%2Ffcitx5-rime%2F)](https://jenkins.fcitx-im.org/job/fcitx5-rime/)

[![Coverity Scan Status](https://img.shields.io/coverity/scan/13835.svg)](https://scan.coverity.com/projects/fcitx-fcitx5-rime)

## one-schema-one-im

This branch exports each schema as a separate input method.

Issues:

- This is probably very broken when RIME has not deployed yet
- Can we prevent schema switching when using an input method that corresponds to just one schema?
  - If not… sure maybe just treat it as a temporary switch.
- Perhaps we can make a version with one input method per language?
