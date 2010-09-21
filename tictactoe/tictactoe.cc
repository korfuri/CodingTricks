// Copyright Uriel Corfa, 2009
//    <uriel at corfa dot fr>
//
// Meta-tic-tac-toe
// This program is a simple demonstration of what you can do in C++
// with a few templates and a lot of time (not that it is long to
// write, but it's fucking long to compile).
//
// A few conventions before explaining how it works :
//
// This tic tac toe use the following grid :
//
//    A0 A1 A2
//    B0 B1 B2
//    C0 C1 C2
//
// Many templates will take these nine values (as ints) as inputs.
// The two players are represented in those ints by values 1 (for X)
// and 2 (for O). 0 (zero) stands for "empty slot".
//
// To potential readers :
// - Sorry for the unclear comments. It's hard to reorganize a code
// like that without having gcc complaining about a lot of stuff.
// - To  potential employers : I  can also make real code.  Drop me an
// email if you are interested :)

#include <iostream>
#include <iomanip>

using namespace std;

// Just a basic meta-if for types.
// Only unusual thing here : we inherit from the response type instead
// of typedef'ing it. That doesn't change anything.

template<bool value, typename ifTrue, typename ifFalse>
class TypeIf : public ifFalse {};

template<typename ifTrue, typename ifFalse>
class TypeIf<true, ifTrue, ifFalse> : public ifTrue {};


// This simple template outputs (in its member NextP) the number of
// the next player to play.

template<int P>
struct EvaluateNextP
{
  static const int NextP = 2;
};
template<> struct EvaluateNextP<2> { static const int NextP = 1; };


// This template class evaluates a given situation.
//    Args :
//    - The 9 ints representing the board
//    - The num. of the last player
//
//    Outputs :
//    - isDefined : 1 if the game is now finished, 0 otherwise
//    - result : 1 if the last player won, 0 otherwise

template<int A0, int A1, int A2,
         int B0, int B1, int B2,
         int C0, int C1, int C2,
         int lastP>
struct isWon
{
 private:
  // For convenience, we'll pre-fetch the other player's number
  static const int otherP = EvaluateNextP<lastP>::NextP;

  // Those are all the possible winning situations
  // 'd' stands for 'diagonal', of course.
  static const bool winA = (A0 == lastP && A1 == lastP && A2 == lastP);
  static const bool winB = (B0 == lastP && B1 == lastP && B2 == lastP);
  static const bool winC = (C0 == lastP && C1 == lastP && C2 == lastP);
  static const bool win0 = (A0 == lastP && B0 == lastP && C0 == lastP);
  static const bool win1 = (A1 == lastP && B1 == lastP && C1 == lastP);
  static const bool win2 = (A2 == lastP && B2 == lastP && C2 == lastP);
  static const bool windiagslash = (A0 == lastP && B1 == lastP && C2 == lastP);
  static const bool windiagbackslash = (C0 == lastP && B1 == lastP && A2 == lastP);

  // And all the losing situations (i.e. the same for the other player)
  static const bool loseA = (A0 == otherP && A1 == otherP && A2 == otherP);
  static const bool loseB = (B0 == otherP && B1 == otherP && B2 == otherP);
  static const bool loseC = (C0 == otherP && C1 == otherP && C2 == otherP);
  static const bool lose0 = (A0 == otherP && B0 == otherP && C0 == otherP);
  static const bool lose1 = (A1 == otherP && B1 == otherP && C1 == otherP);
  static const bool lose2 = (A2 == otherP && B2 == otherP && C2 == otherP);
  static const bool losediagslash = (A0 == otherP && B1 == otherP && C2 == otherP);
  static const bool losediagbackslash = (C0 == otherP && B1 == otherP && A2 == otherP);

 public:
  // End of game is achieved when any of those situations is reached.
  static const bool isDefined = (winA | winB | winC | win0 | win1 | win2 | windiagslash | windiagbackslash |
                                 loseA | loseB | loseC | lose0 | lose1 | lose2 | losediagslash | losediagbackslash);

  // We can also tell who won by evaluating just a half of the values
  static const int result = (
      (
          winA | winB | winC | win0 | win1 | win2 | windiagslash | windiagbackslash
       ) ? 1 : 0);
};

// Forward declaration only.
// See below.

template<int A0, int A1, int A2,
         int B0, int B1, int B2,
         int C0, int C1, int C2,
         int lastP, int turnNum>
struct GamePosition;

// We need a special case for impossible game positions. This class
// terminates the recursion. It also has a score low enough to never
// be reached.

struct ImpossibleGamePosition {
 public:
  static const int isPossible = 0;
  static void printTree(void) {}
  static void printGame(void) {}
  static const int score = -1000000;
};

// This template compares 3 positions and outputs the best one.

template<typename p0, typename p1, typename p2>
struct Best3 {
 private:
  typedef TypeIf<(p0::score < p1::score), p1, p0> _tmp;
 public:
  typedef TypeIf<(_tmp::score < p2::score), p2, _tmp> best;
};

// This template select the best position among 9 (based on their
// "score" variable). It uses the Best3 template defined above to do
// so.

template<typename pA0, typename pA1, typename pA2,
         typename pB0, typename pB1, typename pB2,
         typename pC0, typename pC1, typename pC2>
struct Best {
 private:
  typedef typename Best3<pA0, pA1, pA2>::best _A;
  typedef typename Best3<pB0, pB1, pB2>::best _B;
  typedef typename Best3<pC0, pC1, pC2>::best _C;
 public:
  typedef typename Best3<_A, _B, _C>::best best;
};

// This class is the core of the program. Its role is to, given a
// position (inputs A0 to C2), the number of the player who played
// last, and the number of the last turn :
// - Instantiate the 9 potential next positions
// - Select the best of them
// - Estimate the average score.

template<int A0, int A1, int A2,
         int B0, int B1, int B2,
         int C0, int C1, int C2,
         int lastP, int turnNum>
struct GameEvolution
{
 private:
  // For convenience, we pre-fetch the number of the next player
  static const int NextP = EvaluateNextP<lastP>::NextP;

  // For each slot :
  // - If the slot is empty (its value = 0), instantiate the
  //   GamePosition that would result if we played there.
  // - If it is not, use a ImpossibleGamePosition instead
  // We save each position in a private typedef for future use.
  typedef TypeIf<A0 == 0,GamePosition<A0+NextP,A1,A2,B0,B1,B2,C0,C1,C2,NextP,turnNum+1>,ImpossibleGamePosition> pA0;
  typedef TypeIf<A1 == 0,GamePosition<A0,A1+NextP,A2,B0,B1,B2,C0,C1,C2,NextP,turnNum+1>,ImpossibleGamePosition> pA1;
  typedef TypeIf<A2 == 0,GamePosition<A0,A1,A2+NextP,B0,B1,B2,C0,C1,C2,NextP,turnNum+1>,ImpossibleGamePosition> pA2;
  typedef TypeIf<B0 == 0,GamePosition<A0,A1,A2,B0+NextP,B1,B2,C0,C1,C2,NextP,turnNum+1>,ImpossibleGamePosition> pB0;
  typedef TypeIf<B1 == 0,GamePosition<A0,A1,A2,B0,B1+NextP,B2,C0,C1,C2,NextP,turnNum+1>,ImpossibleGamePosition> pB1;
  typedef TypeIf<B2 == 0,GamePosition<A0,A1,A2,B0,B1,B2+NextP,C0,C1,C2,NextP,turnNum+1>,ImpossibleGamePosition> pB2;
  typedef TypeIf<C0 == 0,GamePosition<A0,A1,A2,B0,B1,B2,C0+NextP,C1,C2,NextP,turnNum+1>,ImpossibleGamePosition> pC0;
  typedef TypeIf<C1 == 0,GamePosition<A0,A1,A2,B0,B1,B2,C0,C1+NextP,C2,NextP,turnNum+1>,ImpossibleGamePosition> pC1;
  typedef TypeIf<C2 == 0,GamePosition<A0,A1,A2,B0,B1,B2,C0,C1,C2+NextP,NextP,turnNum+1>,ImpossibleGamePosition> pC2;

 public:
  // Unused.
  // This simple method prints the whole recursion tree. It may be
  // used for debugging purpose.
  // To be precise, this method just calls the PrintTree method on
  // every possible (and impossible) move. (The
  // ImpossibleGamePosition::printTree method does nothing).
  static void printTree(void) {
    pA0::printTree();
    pA1::printTree();
    pA2::printTree();
    pB0::printTree();
    pB1::printTree();
    pB2::printTree();
    pC0::printTree();
    pC1::printTree();
    pC2::printTree();
  }

  // The score of a possible position is the average of all the
  // possible next positions.
  // The "+1" in the equation avoids a "division by zero" error at the
  // end of the recursion tree, when every slot is taken, making all
  // next position impossible
  static const int score = (((pA0::score * pA0::isPossible) +
                             (pA1::score * pA1::isPossible) +
                             (pA2::score * pA2::isPossible) +
                             (pB0::score * pB0::isPossible) +
                             (pB1::score * pB1::isPossible) +
                             (pB2::score * pB2::isPossible) +
                             (pC0::score * pC0::isPossible) +
                             (pC1::score * pC1::isPossible) +
                             (pC2::score * pC2::isPossible))
                            / (pA0::isPossible +
                               pA1::isPossible +
                               pA2::isPossible +
                               pB0::isPossible +
                               pB1::isPossible +
                               pB2::isPossible +
                               pC0::isPossible +
                               pC1::isPossible +
                               pC2::isPossible + 1));


  // We publicly typedef the best move to play.
  // That way, to know what to play, you just need to do :
  //   GameEvolution<params>::best
  // and use the given class.
  // It is NOT guaranteed to be a possible game position. If the board
  // is full (and the game over), every possible move will be a
  // ImpossibleGamePosition. Then "best" will just be one of them.
  typedef typename Best<pA0, pA1, pA2, pB0, pB1, pB2, pC0, pC1, pC2>::best best;
};

// Given a player number, outputs 'X' or 'O'. If the player is 0
// (empty slot), outputs '.'

template<int player>
struct PlayerChar {
 public:
  static const char c = '?';
};
template<>
struct PlayerChar<1> {
 public:
  static const char c = 'X';
};
template<>
struct PlayerChar<2> {
 public:
  static const char c = 'O';
};


// This template is used when printing the possible moves tree.
// We need a special behaviour when one of the players won. This
// template takes 2 arguments that should be the values of
// IsWon<position>::isDefined and idem::isWon

// General case : just use the GameEvolution::printTree method to call
// the GamePosition::printTree of every child node in the tree
template<int isDefined, int isWon,
         int A0, int A1, int A2,
         int B0, int B1, int B2,
         int C0, int C1, int C2,
         int lastP, int turnNum>
struct Printer {
  static void printTree(void) {
    GameEvolution<A0,A1,A2,B0,B1,B2,C0,C1,C2,lastP,turnNum>::printTree();
  }
};

// Losing move : this is impossible.
// You can't lose right after playing. Either you win, or the game
// continues.
template<int A0, int A1, int A2,
         int B0, int B1, int B2,
         int C0, int C1, int C2,
         int lastP, int turnNum>
struct Printer<1, 0, A0, A1, A2, B0, B1, B2, C0, C1, C2, lastP, turnNum> {
  static void printTree(void) {
    // You can abort() here.
    cout << "Winner: Player " << PlayerChar<EvaluateNextP<lastP>::NextP>::c << endl;
  }
};

// Winning move : display a message with the player's symbol
template<int A0, int A1, int A2,
         int B0, int B1, int B2,
         int C0, int C1, int C2,
         int lastP, int turnNum>
struct Printer<1, 1, A0, A1, A2, B0, B1, B2, C0, C1, C2, lastP, turnNum> {
  static void printTree(void) {
    cout << "Winner: Player " << PlayerChar<lastP>::c << endl;
  }
};


// Defines a potential position.
//  Inputs :
//  + the 9 states defining the game board
//  + the number of the player who just moved
//  + the current turn number (used for indentation)
//
//  Outputs :
//  + Can this player still win ?
//  + Method printTree : prints the position and recurses to do the
//    same with all its possible evolutions
template<int A0, int A1, int A2,
         int B0, int B1, int B2,
         int C0, int C1, int C2,
         int lastP, int turnNum>
struct GamePosition
{
 private:
  // We save the "winning or not" state of this position in _IsWon
  typedef isWon<A0,A1,A2,B0,B1,B2,C0,C1,C2,lastP> _isWon;
  // And for convenience we refer to the evolutions with _GameEvolution
  typedef GameEvolution<A0,A1,A2,B0,B1,B2,C0,C1,C2,lastP,turnNum> _GameEvolution;

 public:
  // This member is used to differ from ImpossibleGamePosition
  static const int isPossible = 1;

  // The score of a position is either :
  // - Winning position : +100
  // - Losing position : -100
  // - None of the abobe : the mean of the scores of the other
  // possible games. Note the minus : the score for the next positions
  // is calculated for the other player, we have to reverse it.
  static const int score = (_isWon::isDefined ? (_isWon::result ? 100 : -100) : (- _GameEvolution::score));

  // Prints the board. We use setfill/setw to indentate depending on
  // the turn number.
  static void printBoard(void) {
    cout << setfill(' ') <<
        setw(turnNum) << 0 << setw(0) << PlayerChar<A0>::c << PlayerChar<A1>::c << PlayerChar<A2>::c << endl <<
        setw(turnNum) << 1 << setw(0) << PlayerChar<B0>::c << PlayerChar<B1>::c << PlayerChar<B2>::c << endl <<
        setw(turnNum) << 2 << setw(0) << PlayerChar<C0>::c << PlayerChar<C1>::c << PlayerChar<C2>::c << endl << endl;
  }

  // Print the board, and either recurse or display a message
  // celebrating victory
  static void printTree(void) {
    printBoard();
    Printer<isWon<A0,A1,A2,B0,B1,B2,C0,C1,C2,lastP>::isDefined,
        isWon<A0,A1,A2,B0,B1,B2,C0,C1,C2,lastP>::result,
        A0,A1,A2,B0,B1,B2,C0,C1,C2,lastP,turnNum>::printTree();
  }

  // Display the board and play next turn
  static void printGame(void) {
    printBoard();
    _GameEvolution::best::printGame();
  }
};




// And go :)

int main(void)
{
  /* Instantiate the first position :
               vvvvvvvvvvvvvvvvv-empty board
                                 v-'X' plays first
                                   v-turn no. 1 */
  GamePosition<0,0,0,0,0,0,0,0,0,1,1>::printGame();
  return 0;
}
