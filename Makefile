.PHONY: fcitx5-rime librime

.default: fcitx5-rime

fcitx5-rime: librime
	emacs --script apply-template.el fcitx5-rime
	cd rebranded-fcitx5-rime && makepkg -s
	mv rebranded-fcitx5-rime/fcitx5-*.zst .

librime:
	emacs --script apply-template.el librime

install: all
	sudo pacman -U fcitx5-*.zst lib*.zst
