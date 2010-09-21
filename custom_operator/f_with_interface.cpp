#include <iostream>

class IIntermediary;

class IHasDoubleArrow
{
public:
  virtual ~IHasDoubleArrow(void);
  virtual IIntermediary const& operator--(int) = 0;
  template <typename T, typename U>  T& operator_darrow(U const& x) = 0;
};

class IIntermediary
{
public:
  virtual ~IIntermediary(void);
  template<typename U> virtual IHasDoubleArrow& operator>(U const& x) const = 0;
};


template <typename T>
class Intermediary : public IIntermediary
{
private:
  Intermediary(Intermediary const&);
  Intermediary& operator=(Intermediary const&);
  Intermediary();

  T& _t;
public:
  Intermediary(T& t) : _t(t) {};

  template<typename U>T& operator>(U const& x) const { return _t.operator_darrow(x); }
};

template <typename T>
class Intermediary_const
{
private:
  Intermediary_const(Intermediary_const const&);
  Intermediary_const& operator=(Intermediary_const const&);
  Intermediary_const();

  T const& _t;
public:
  Intermediary_const(T& t) : _t(t) {};

  template<typename U>T const& operator>(U const& x) const { return _t.operator_darrow(x); }
};

class Test : IHasDoubleArrow
{
private:
  Test(Test const&);
  Test& operator=(Test const&);

public:
  friend class Intermediary<Test>;
  friend class Intermediary_const<Test>;

  Test() : _tt(*new Intermediary<Test>(*this)),
	   _ttc(*new Intermediary_const<Test>(*this)) {}
  ~Test() { delete &_tt; delete &_ttc; }

  Intermediary<Test> const& operator--(int) { return _tt; }
  Intermediary_const<Test> const& operator--(int) const { return _ttc; }

private:
  Intermediary<Test>& _tt;
  Intermediary_const<Test>& _ttc;
  template <typename T>  Test& operator_darrow(T const& x) { std::cout << "Test --> " << x << std::endl; return *this; }
  template <typename T>  Test const& operator_darrow(T const& x) const { std::cout << "Test const --> " << x << std::endl; return *this; }
};

int	main(void)
{
  Test t;

  t-->42;
  t-->"lol";

  Test const& tc = t;
  tc-->43;
  tc-->"bla";
}
