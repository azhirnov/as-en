#!/data/data/com.termux/files/usr/bin/bash

set -u

is_yes() {
  case "$1" in
    Y|y)
      return 0
      ;;
    *)
      return 1
      ;;
  esac
}

main() {
  if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "You have uncommitted changes:"
    git status --short

    printf 'force reset? [y/N] '
    read -r answ

    if is_yes "$answ"; then
      echo "force reset to origin"
      git reset --hard
      git pull --force
      return 0
    fi

    return 1
  fi

  git pull
}

cd /sdcard/Code/AE
main
git log -1


printf 'update shortcuts? [y/N]'
read -r answ

if is_yes "$answ"; then
	cp -f "/sdcard/Code/AE/build_scripts/android_termux/build.sh" ~/.shortcuts/ae.build
	cp -f "/sdcard/Code/AE/build_scripts/android_termux/build-targ.sh" ~/.shortcuts/ae.run
	cp -f "/sdcard/Code/AE/build_scripts/android_termux/git-pull.sh" ~/.shortcuts/ae.pull
	cp -f "/sdcard/Code/AE/build_scripts/android_termux/git-push.sh" ~/.shortcuts/ae.push
	echo "updated"
fi

printf 'exit? [Y/n] '
read -r answ
answ="${answ:-y}"

if is_yes "$answ"; then
  exit 0
fi

exec bash

