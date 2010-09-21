#include <iostream>
#define NB_COLS 8 /* Set this to less than 8 to compile faster and solve a N-queens problem */

template<typename Parent>
class InstantiatePosition;

template<bool value, typename ifTrue, typename ifFalse>
struct _if : public ifTrue {};
template<typename ifTrue, typename ifFalse>
struct _if<false, ifTrue, ifFalse> : public ifFalse {};

class ImpossiblePosition {
 public:
  static void solve(void) {}
};

template<typename Parent>
class Printer {
 public:
  static void solve(void) {
    std::cout << Parent::col1 << Parent::col2 << Parent::col3 <<
        Parent::col4 << Parent::col5 << Parent::col6 <<
        Parent::col7 << Parent::col8 << std::endl;
  }
};

template<int column, typename T>
class ValueOfColumn { public: static const int value = 0; };
template<typename T> class ValueOfColumn<1, T> { public: static const int value = T::col1; };
template<typename T> class ValueOfColumn<2, T> { public: static const int value = T::col2; };
template<typename T> class ValueOfColumn<3, T> { public: static const int value = T::col3; };
template<typename T> class ValueOfColumn<4, T> { public: static const int value = T::col4; };
template<typename T> class ValueOfColumn<5, T> { public: static const int value = T::col5; };
template<typename T> class ValueOfColumn<6, T> { public: static const int value = T::col6; };
template<typename T> class ValueOfColumn<7, T> { public: static const int value = T::col7; };
template<typename T> class ValueOfColumn<8, T> { public: static const int value = T::col8; };

template<int column, typename T>
class IsPossible {
 private:
  typedef IsPossible<column - 1, T> next;
 public:
  static const bool isPossible = next::isPossible &&
      ValueOfColumn<T::column, T>::value != ValueOfColumn<column, T>::value &&
      ValueOfColumn<T::column, T>::value != ValueOfColumn<column, T>::value + (T::column - column) &&
      ValueOfColumn<T::column, T>::value != ValueOfColumn<column, T>::value - (T::column - column) ;
};

template<typename T>
class IsPossible<0, T> {
 public:
  static const bool isPossible = true;
};

template<typename Parent, int WhereToPlay>
class Position {
 public:
  static const int column = Parent::column + 1;

  static const int col1 = column == 1 ? WhereToPlay : Parent::col1;
  static const int col2 = column == 2 ? WhereToPlay : Parent::col2;
  static const int col3 = column == 3 ? WhereToPlay : Parent::col3;
  static const int col4 = column == 4 ? WhereToPlay : Parent::col4;
  static const int col5 = column == 5 ? WhereToPlay : Parent::col5;
  static const int col6 = column == 6 ? WhereToPlay : Parent::col6;
  static const int col7 = column == 7 ? WhereToPlay : Parent::col7;
  static const int col8 = column == 8 ? WhereToPlay : Parent::col8;

  static void solve(void) { children_::solve(); }

  typedef Parent parent_;

 private:
  typedef Position<Parent, WhereToPlay> self_;
  static const bool isPossible = IsPossible<column - 1, self_>::isPossible;
  typedef _if<isPossible, _if<column == NB_COLS, Printer<self_>, InstantiatePosition<self_> >, ImpossiblePosition> children_;
};

template<typename Parent>
class InstantiatePosition {
 private:
  typedef Position<Parent, 1> p1;
  typedef Position<Parent, 2> p2;
  typedef Position<Parent, 3> p3;
  typedef Position<Parent, 4> p4;
  typedef Position<Parent, 5> p5;
  typedef Position<Parent, 6> p6;
  typedef Position<Parent, 7> p7;
  typedef Position<Parent, 8> p8;

 public:
  static void solve(void) {
    p1::solve();
    p2::solve();
    p3::solve();
    p4::solve();
    p5::solve();
    p6::solve();
    p7::solve();
    p8::solve();
  }
};

class Start {
 private:
  typedef InstantiatePosition<Start> recurser_;

 public:
  static const int col1 = 0;
  static const int col2 = 0;
  static const int col3 = 0;
  static const int col4 = 0;
  static const int col5 = 0;
  static const int col6 = 0;
  static const int col7 = 0;
  static const int col8 = 0;

  static const int column = 0;

  static void solve(void) { recurser_::solve(); }
};

int main(void) {
  Start::solve();
};
