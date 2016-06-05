#!/usr/bin/env bash
set -o pipefail
[[ "${DEBUG:-}" ]] && set -x

declare -i failed
failed=0

success() {
  printf "\r\033[2K  [ \033[00;32mOK\033[0m ] Checking %s...\n" "$1"
}

fail() {
	printf "\r\033[2K  [\033[0;31mFAIL\033[0m] Checking %s...\n" "$1"
	failed=1
}

check() {
  local script="$1"
  if shellcheck "$script"; then
	success "$script"
  else
	fail "$script"
  fi
}

find_prunes() {
  local prunes="! -path './.git/*'"
  if [ -f .gitmodules ]; then
    while read -r module; do
      prunes="$prunes ! -path './$module/*'"
    done < <(grep path .gitmodules | awk '{print $3}')
  fi
  echo "$prunes"
}

find_cmd() {
  echo "find . -type f -and \( -perm /111 -or -name '*.sh' \) $(find_prunes)"
}

check_all_executables() {
  echo "Checking executables and .sh files..."
  while read -r script; do
    head=$(head -n1 "$script")
    [[ "$head" =~ .*ruby.* ]] && continue
    [[ "$head" =~ .*zsh.* ]] && continue
    [[ "$head" =~ ^#compdef.* ]] && continue
    check "$script"
  done < <(eval "$(find_cmd)")
  exit $failed
}

check_all_executables
