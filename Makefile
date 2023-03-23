.PHONY: fcitx5-rime librime

fcitx5-rime:
	emacs --script apply-template.el fcitx5-rime
	cd rebranded-fcitx5-rime && makepkg -s
	mv rebranded-fcitx5-rime/fcitx5-*.zst .

librime:
	emacs --script apply-template.el librime
	cd rebranded-librime && makepkg -s
	mv rebranded-librime/lib*.zst .

install: fcitx5-rime librime
	sudo pacman -U fcitx5-*.zst lib*.zst
