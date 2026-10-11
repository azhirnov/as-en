#!/data/data/com.termux/files/usr/bin/bash

set -uo pipefail

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
    cd /sdcard/Code/AE

    if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
      echo "Error: this is not a git repository."
      return 1
    fi

	if git diff --quiet && git diff --cached --quiet && [[ -z "$(git ls-files --others --exclude-standard)" ]]; then
      echo "No changes to commit."
      return 0
    fi

    echo "Changed files:"
    git status --short

    echo
    read -r -p "Show all changes? (q to close diff) [y/N] " show_changes
    if is_yes "$show_changes"; then
      git diff
      echo
      echo "Untracked files:"
      git ls-files --others --exclude-standard
    fi

    echo
    read -r -p "Commit all changes? [y/N] " commit_all
    if is_yes "$commit_all"; then
      echo
    else
      echo "Aborted before commit."
      return 0
    fi

    git add -A

    echo
    read -r -p "Commit message: " commit_message
    if [[ -z "${commit_message// }" ]]; then
      echo "Error: commit message cannot be empty."
      return 1
    fi

    git commit -m "$commit_message"

    echo
    read -r -p "Push commit? [y/N] " push_commit
    if is_yes "$push_commit"; then
      git push
    else
      echo "Committed locally. Push skipped."
    fi
}

main

printf 'exit? [Y/n] '
read -r answ
answ="${answ:-y}"

if is_yes "$answ"; then
  exit 0
fi

exec bash
