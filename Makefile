archpkg:
	emacs --script apply-template.el
	cd rebranded && makepkg -s
	mv rebranded/fcitx5-*.zst .

install: archpkg
	sudo pacman -U fcitx5-*.zst