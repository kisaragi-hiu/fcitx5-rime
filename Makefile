.PHONY: fcitx5-rime librime

fcitx5-rime:
	emacs --script apply-template.el template
	cd rebranded-template && makepkg -s
	mv rebranded-template/fcitx5-*.zst .

librime:
	emacs --script apply-template.el librime
	cd rebranded-librime && makepkg -s
	mv rebranded-librime/lib*.zst .

install: fcitx5-rime
	sudo pacman -U fcitx5-*.zst
