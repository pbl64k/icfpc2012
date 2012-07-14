
#define DEV

#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>

#include <cctype>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include "PressAnyKey.h"

using namespace std;

bool interactive;

string sol;

void terminate(int signal)
{
	signal = signal;

	cout << sol << "A" << endl;

	exit(EXIT_SUCCESS);
}

class bd_map
{
	public:
	int n_, m_, i_;
	int r_x_, r_y_;
	int l_x_, l_y_;
	int ls_;
	bool last_n_;
	vector<vector<char> > x_;
	set<pair<int, int> > lambdae_;

	bd_map(): n_(0), m_(0), i_(0),
			r_x_(0), r_y_(0), l_x_(0), l_y_(0),
			ls_(0), last_n_(true),
			x_(vector<vector<char> >(1)),
			lambdae_(set<pair<int, int> >())
	{
		x_.push_back(vector<char>(0));
	}

	bool add(char c)
	{
		switch (c)
		{
			case '\\':
				++ls_;
			case '#':
			case ' ':
			case '.':
			case '*':
			case 'R':
			case 'L':
				if (c == 'R')
				{
					r_x_ = i_;
					r_y_ = m_;
				}
				if (c == 'L')
				{
					l_x_ = i_;
					l_y_ = m_;
				}
				x_[m_].push_back(c);
				n_ = max(n_, i_);
				++i_;
				last_n_ = false;
				return false;
			case '\n':
				if (last_n_)
				{
					return true;
				}
				x_.push_back(vector<char>());
				++m_;
				i_ = 0;
				last_n_ = true;
				return false;
			default:
				return false;
		}
	}

	void fin_init()
	{
		--m_;
		r_y_ = m_ - r_y_;
		l_y_ = m_ - l_y_;

		for (int i = 0; i <= m_; ++i)
		{
			for (int j = 0; j <= n_; ++j)
			{
				if ((*this)(j, i) == '\\')
				{
					lambdae_.insert(make_pair(j, i));
				}
			}
		}
	}

	void disp()
	{
		cout << "Playing field: " << (n_ + 1) << "x" << (m_ + 1) << endl;
		cout << "Robot at (" << r_x_ << ", " << r_y_ << "), ";
		cout << "exit at (" << l_x_ << ", " << l_y_ << ")" << endl;
		for (int i = m_; i >= 0; --i)
		{
			for (int j = 0; j <= n_; ++j)
			{
				cout << (*this)(j, i);
			}
			cout << endl;
		}
		cout << "OK" << endl;
	}

	char &operator()(int x, int y)
	{
		if (! valid(x, y))
		{
			throw out_of_range("D'oh.");
		}

		return x_[m_ - y][x];
	}

	bool move(char c)
	{
		switch (c)
		{
			case 'L':
				return l();
			case 'R':
				return r();
			case 'U':
				return u();
			case 'D':
				return d();
			case 'W':
				return w();
			case 'A':
				return w();
			default:
				throw invalid_argument("Huh?");
		}
	}

	bool l()
	{
		int nx = r_x_ - 1, ny = r_y_;

		if (! valid(nx, ny))
		{
			return w();
		}

		bool r = false, ok = false;

		switch ((*this)(nx, ny))
		{
			case '\\':
				r = true;
			case ' ':
			case '.':
			case 'O':
				ok = true;
				break;
			case '*':
				if (valid(nx - 1, ny) && (*this)(nx - 1, ny) == ' ')
				{
					(*this)(nx - 1, ny) = '*';
					ok = true;
				}
		}

		if (ok)
		{
			(*this)(r_x_, r_y_) = ' ';
			r_x_ = nx; r_y_ = ny;
			(*this)(r_x_, r_y_) = 'R';
			return r;
		}

		return w();
	}

	bool r()
	{
		int nx = r_x_ + 1, ny = r_y_;

		if (! valid(nx, ny))
		{
			return w();
		}

		bool r = false, ok = false;

		switch ((*this)(nx, ny))
		{
			case '\\':
				r = true;
			case ' ':
			case '.':
			case 'O':
				ok = true;
				break;
			case '*':
				if (valid(nx + 1, ny) && (*this)(nx + 1, ny) == ' ')
				{
					(*this)(nx + 1, ny) = '*';
					ok = true;
				}
		}

		if (ok)
		{
			(*this)(r_x_, r_y_) = ' ';
			r_x_ = nx; r_y_ = ny;
			(*this)(r_x_, r_y_) = 'R';
			return r;
		}

		return w();
	}

	bool u()
	{
		int nx = r_x_, ny = r_y_ + 1;

		if (! valid(nx, ny))
		{
			return w();
		}

		bool r = false, ok = false;

		switch ((*this)(nx, ny))
		{
			case '\\':
				r = true;
			case ' ':
			case '.':
			case 'O':
				ok = true;
		}

		if (ok)
		{
			(*this)(r_x_, r_y_) = ' ';
			r_x_ = nx; r_y_ = ny;
			(*this)(r_x_, r_y_) = 'R';
			return r;
		}

		return w();
	}

	bool d()
	{
		int nx = r_x_, ny = r_y_ - 1;

		if (! valid(nx, ny))
		{
			return w();
		}

		bool r = false, ok = false;

		switch ((*this)(nx, ny))
		{
			case '\\':
				r = true;
			case ' ':
			case '.':
			case 'O':
				ok = true;
		}

		if (ok)
		{
			(*this)(r_x_, r_y_) = ' ';
			r_x_ = nx; r_y_ = ny;
			(*this)(r_x_, r_y_) = 'R';
			return r;
		}

		return w();
	}

	bool w()
	{
		return false;
	}

	bool valid(int x, int y)
	{
		if (x < 0 || x > n_ || y < 0 || y > m_)
		{
			return false;
		}

		return true;
	}

	pair<int, int> get_n_l()
	{
		pair<int, int> r = make_pair(l_x_, l_y_);
		int r_dist = 1000000;
		int c_dist;

		for (set<pair<int, int> >::iterator l_iter = lambdae_.begin();
				l_iter != lambdae_.end(); ++l_iter)
		{
			c_dist = abs(r_x_ - l_iter->first) + abs(r_y_ - l_iter->second);

			if (c_dist < r_dist)
			{
				r = make_pair(l_iter->first, l_iter->second);
				r_dist = c_dist;
			}
		}

		return r;
	}

	int get_c_dist()
	{
		pair<int, int> nl = get_n_l();

		return abs(r_x_ - nl.first) + abs(r_y_ - nl.second);
	}
};

class bd_game
{
	public:
	bool finished_;
	bool escaped_;
	bool died_;
	bool aborted_;
	bool changed_;
	int ls_;
	int sc_;
	bd_map m_;

	bd_game(): finished_(false), escaped_(false), died_(false), aborted_(false),
			changed_(false),
			ls_(0), sc_(0), m_(bd_map())
	{
	}

	void disp()
	{
		pair<int, int> nl = m_.get_n_l();

		cout << "Score: " << sc_ << " Lambdas: " << ls_ << " (" << m_.ls_ << " remaining)" << endl;
		cout << "Target: (" << nl.first << ", " << nl.second << ")" << endl;
		m_.disp();

		if (escaped_)
		{
			cout << "ESCAPED!" << endl;
		}
		else if (died_)
		{
			cout << "DIED." << endl;
		}
		else if (aborted_)
		{
			cout << "ABORTED." << endl;
		}
	}

	void move(char c)
	{
		--sc_;
		if (m_.move(c))
		{
			--m_.ls_;
			m_.lambdae_.erase(make_pair(m_.r_x_, m_.r_y_));
			++ls_;
			sc_ += 25;
		}
		update();
		end(c);
	}

	void update()
	{
		bd_map nm = m_;

		changed_ = false;

		for (int i = 0; i <= m_.m_; ++i)
		{
			for (int j = 0; j <= m_.n_; ++j)
			{
				if (m_.valid(j, i - 1) && m_(j, i) == '*' && m_(j, i - 1) == ' ')
				{
					nm(j, i) = ' ';
					nm(j, i - 1) = '*';
					changed_ = true;
					if (j == m_.r_x_ && i - 1 == m_.r_y_ + 1)
					{
						died_ = true;
					}
				}
				else if (m_.valid(j, i - 1) && m_.valid(j + 1, i) && /* m_.valid(j + 1, i - 1) && */
						m_(j, i) == '*' && m_(j, i - 1) == '*' && m_(j + 1, i) == ' ' && m_(j + 1, i - 1) == ' ')
				{
					nm(j, i) = ' ';
					nm(j + 1, i - 1) = '*';
					changed_ = true;
					if (j + 1 == m_.r_x_ && i - 1 == m_.r_y_ + 1)
					{
						died_ = true;
					}
				}
				else if (m_.valid(j, i - 1) && m_.valid(j + 1, i) && m_.valid(j - 1, i) && /* some obviously true checks skipped */
						m_(j, i) == '*' && m_(j, i - 1) == '*' &&
						(m_(j + 1, i) != ' ' || m_(j + 1, i - 1) != ' ') && m_(j - 1, i) == ' ' && m_(j - 1, i - 1) == ' ')
				{
					nm(j, i) = ' ';
					nm(j - 1, i - 1) = '*';
					changed_ = true;
					if (j - 1 == m_.r_x_ && i - 1 == m_.r_y_ + 1)
					{
						died_ = true;
					}
				}
				else if (m_.valid(j, i - 1) && m_.valid(j + 1, i) && /* m_.valid(j + 1, i - 1) && */
						m_(j, i) == '*' && m_(j, i - 1) == '\\' && m_(j + 1, i) == ' ' && m_(j + 1, i - 1) == ' ')
				{
					nm(j, i) = ' ';
					nm(j + 1, i - 1) = '*';
					changed_ = true;
					if (j + 1 == m_.r_x_ && i - 1 == m_.r_y_ + 1)
					{
						died_ = true;
					}
				}
			}
		}

		if (m_.ls_ == 0)
		{
			nm(m_.l_x_, m_.l_y_) = 'O';
			changed_ = true;
		}

		m_ = nm;
	}

	void end(char c)
	{
		if (m_.r_x_ == m_.l_x_ && m_.r_y_ == m_.l_y_)
		{
			escaped_ = true;
			finished_ = true;

			sc_ += ls_ * 50;

			return;
		}

		if (c == 'A')
		{
			aborted_ = true;
			finished_ = true;

			++sc_;
			sc_ += ls_ * 25;

			return;
		}

		if (died_)
		{
			finished_ = true;
		}
	}
};

int cost_func(bd_game &g1, bd_game &g2)
{
	if (g2.died_)
	{
		return -1000000;
	}

	if (g2.ls_ > g1.ls_)
	{
		return 25;
	}

	if (g2.m_.get_c_dist() < g1.m_.get_c_dist())
	{
		return 1;
	}

	if (g1.m_.r_x_ != g2.m_.r_x_ || g1.m_.r_y_ != g2.m_.r_y_ || g2.changed_)
	{
		return 0;
	}

	return -1000000;
}

// TODO: lookahead.
class bd_robo
{
	public:
	bd_game &g_;

	bd_robo(bd_game &g): g_(g)
	{
	}

	bd_game emulate(char m)
	{
		bd_game g_prime = g_;

		g_prime.move(m);

		return g_prime;
	}

	void solve()
	{
		char m;

		m = pick_a_move();

		sol += m;

		g_.move(m);
	}

	char pick_a_move()
	{
		vector<char> res(0);
		res.push_back('A');
		int res_f = -1000;

		if (g_.sc_ < -1024)
		{
			return 'A';
		}

		char m[6] = "LRUDW";

		bd_game g0;

		for (int i = 0; i < 5; ++i)
		{
			g0 = emulate(m[i]);

			int f = cost_func(g_, g0);

			if (f > res_f)
			{
				res_f = f;
				res = vector<char>();
				res.push_back(m[i]);
			}
			else if (f == res_f)
			{
				res.push_back(m[i]);
			}
		}

		int ix = rand() % res.size();

		return res[ix];
	}
};

int main(int argc, char **argv)
{
	signal(SIGINT, &terminate);

	srand(time(NULL));

	ifstream is;

	if (argc > 1)
	{
		interactive = true;
		is.open(argv[1]);
	}
	else
	{
		interactive = false;
	}

	bd_game g;

	char c;

	(interactive ? is : cin).unsetf(ios_base::skipws);

	while (true)
	{
		(interactive ? is : cin) >> c;

		if ((interactive ? is : cin).eof())
		{
			break;
		}

		if (g.m_.add(c))
		{
			break;
		}
	}

	g.m_.fin_init();

	if (interactive)
	{
		cin.setf(ios_base::skipws);
	
		while (! g.finished_)
		{
			g.disp();
	
			c = toupper(PressAnyKey(""));

			switch (c)
			{
				case 'H':
					c = 'L';
					break;
				case 'J':
					c = 'D';
					break;
				case 'K':
					c = 'U';
					break;
				case 'L':
					c = 'R';
					break;
			}
	
			g.move(c);
		}

		g.disp();
	}
	else
	{
		while (! g.finished_)
		{
			bd_robo r(g);

			r.solve();
		}

#ifdef DEV
		g.disp();
#endif

		cout << sol << endl;
	}

	return 0;
}

