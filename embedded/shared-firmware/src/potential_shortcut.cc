#include "utils/potential_shortcut.h"

[[nodiscard]] bool PotentialShortcut::operator==(
    const PotentialShortcut& other) const {
  return std::hash<PotentialShortcut>()(*this) ==
         std::hash<PotentialShortcut>()(other);
}