/*

3.3.4 - 1D Segment Tree with Range Updates (Fast, No Recursion)

Description: A segment tree is a data structure used for
solving the dynamic range query problem, which asks to
determine the minimum (or maximum) value in any given
range in an array that is constantly being updated.
Lazy propagation is a technique applied to segment trees that
allows range updates to be carried out in O(log(N)) time.

Time Complexity: Assuming merge() is O(1), query(), update(),
at() are O(log(N)). If merge() is not constant time, then all
running times are multiplied by whatever complexity the merge
function runs in.

Space Complexity: O(N) on the size of the array.

Note: This implementation is 0-based, meaning that all
indices from 0 to T.size() - 1, inclusive, are accessible.

*/

#include <algorithm> /* std::fill() */
#include <stdexcept> /* std::runtime_error */

template<class T> class segment_tree {
  //Modify the following 5 functions to implement your custom
  //operations on the tree. This implements the Add/Max operations.
  //Operations like Add/Sum, Set/Max can also be implemented.
  static inline const T modify_op(const T & x, const T & y) {
    return x + y;
  }

  static inline const T query_op(const T & x, const T & y) {
    return std::max(x, y);
  }

  static inline const T delta_on_segment(const T & delta, int seglen) {
    if (delta == nullv()) return nullv();
    //Here you must write a fast equivalent of following slow code:
    //  T result = delta;
    //  for (int i = 1; i < seglen; i++) result = query_op(result, delta);
    //  return result;
    return delta;
  }

  static inline const T nullv() { return 0; }
  static inline const T initv() { return 0; }

  T *value, *delta;
  int length, *len;

  static T join_value_with_delta(const T & val, const T & delta) {
    return delta == nullv() ? val : modify_op(val, delta);
  }

  static T join_deltas(const T & delta1, const T & delta2) {
    if (delta1 == nullv()) return delta2;
    if (delta2 == nullv()) return delta1;
    return modify_op(delta1, delta2);
  }

  T join_value_with_delta(int i) {
    return join_value_with_delta(value[i], delta_on_segment(delta[i], len[i]));
  }

  void push_delta(int i) {
    int d = 0;
    while ((i >> d) > 0) d++;
    for (d -= 2; d >= 0; d--) {
      int x = i >> d;
      value[x >> 1] = join_value_with_delta(x >> 1);
      delta[x] = join_deltas(delta[x], delta[x >> 1]);
      delta[x ^ 1] = join_deltas(delta[x ^ 1], delta[x >> 1]);
      delta[x >> 1] = nullv();
    }
  }

 public:
  segment_tree(int N) {
    length = N;
    value = new T[2*N];
    delta = new T[2*N];
    len = new int[2*N];
    std::fill(delta, delta + 2*N, nullv());
    std::fill(len + N, len + 2*N, 1);
    for (int i = 0; i < N; i++) value[i + N] = initv();
    for (int i = 2*N - 1; i > 1; i -= 2) {
      value[i >> 1] = query_op(value[i], value[i ^ 1]);
      len[i >> 1] = len[i] + len[i ^ 1];
    }
  }

  ~segment_tree() {
    delete[] value;
    delete[] delta;
    delete[] len;
  }

  int size() { return length; }

  T query(int lo, int hi) {
    if (lo < 0 || hi >= length || lo > hi)
      throw std::runtime_error("Invalid query range.");
    push_delta(lo += length);
    push_delta(hi += length);
    T res = 0;
    bool found = false;
    for (; lo <= hi; lo = (lo + 1) >> 1, hi = (hi - 1) >> 1) {
      if ((lo & 1) != 0) {
        res = found ? query_op(res, join_value_with_delta(lo)) :
                      join_value_with_delta(lo);
        found = true;
      }
      if ((hi & 1) == 0) {
        res = found ? query_op(res, join_value_with_delta(hi)) :
                      join_value_with_delta(hi);
        found = true;
      }
    }
    if (!found) throw std::runtime_error("Not found.");
    return res;
  }

  void modify(int lo, int hi, const T & delta) {
    if (lo < 0 || hi >= length || lo > hi)
      throw std::runtime_error("Invalid modify range.");
    push_delta(lo += length);
    push_delta(hi += length);
    int ta = -1, tb = -1;
    for (; lo <= hi; lo = (lo + 1) >> 1, hi = (hi - 1) >> 1) {
      if ((lo & 1) != 0) {
        this->delta[lo] = join_deltas(this->delta[lo], delta);
        if (ta == -1) ta = lo;
      }
      if ((hi & 1) == 0) {
        this->delta[hi] = join_deltas(this->delta[hi], delta);
        if (tb == -1) tb = hi;
      }
    }
    for (int i = ta; i > 1; i >>= 1)
      value[i >> 1] = query_op(join_value_with_delta(i),
                               join_value_with_delta(i ^ 1));
    for (int i = tb; i > 1; i >>= 1)
      value[i >> 1] = query_op(join_value_with_delta(i),
                               join_value_with_delta(i ^ 1));
  }
};

/*** Example Usage ***/

#include <iostream>
using namespace std;

int main() {
  segment_tree<int> T(10);
  T.modify(0, 0, 10);
  T.modify(1, 1, 5);
  T.modify(1, 1, 4);
  T.modify(2, 2, 7);
  T.modify(3, 3, 8);
  cout << T.query(0, 3) << "\n"; //10
  cout << T.query(1, 3) << "\n"; //9
  T.modify(0, 9, 5);
  cout << T.query(0, 9) << "\n"; //15
  return 0;
}
