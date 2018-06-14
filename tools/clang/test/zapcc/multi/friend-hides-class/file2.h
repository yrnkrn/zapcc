#pragma once
namespace FriendHidesNamespace {
struct FormattedNumber {
  friend class FriendHidesClass;
};
}
