#pragma once
struct Bug1016BlockVector { Bug1016BlockVector operator=(double); };
struct Bug1016ConstraintMatrix {
  void condense(Bug1016BlockVector &);
  void condense(Bug1016BlockVector &, Bug1016BlockVector &c){ c = 0.; }
};
