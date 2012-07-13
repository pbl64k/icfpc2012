
#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>

#include <cstdlib>

#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include <signal.h>

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
	vector<vector<char> > x_;

	bd_map(): n_(0), m_(0), i_(0),
			r_x_(0), r_y_(0), l_x_(0), l_y_(0),
			x_(vector<vector<char> >(1))
	{
		x_.push_back(vector<char>(0));
	}

	void add(char c)
	{
		switch (c)
		{
			case '#':
			case ' ':
			case '.':
			case '*':
			case '\\':
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
				break;
			case '\n':
				x_.push_back(vector<char>());
				++m_;
				i_ = 0;
				break;
			default:
				return;
		}
	}

	void fin_init()
	{
		--m_;
		r_y_ = m_ - r_y_;
		l_y_ = m_ - l_y_;
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
			case 'L':
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
			case 'L':
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
			case 'L':
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
			case 'L':
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
};

class bd_game
{
	public:
	bool finished_;
	bool escaped_;
	bool died_;
	bool aborted_;
	int ls_;
	int sc_;
	bd_map m_;

	bd_game(): finished_(false), escaped_(false), died_(false), aborted_(false),
			ls_(0), sc_(0), m_(bd_map())
	{
	}

	void disp()
	{
		cout << "Score: " << sc_ << " Lambdas: " << ls_ << endl;
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
			++ls_;
			sc_ += 25;
		}
		update();
		end(c);
	}

	void update()
	{
	}

	void end(char c)
	{
		if (m_.r_x_ == m_.l_x_ && m_.r_y_ == m_.l_y_)
		{
			escaped_ = true;
			finished_ = true;

			return;
		}

		// TODO: died

		if (c == 'A')
		{
			aborted_ = true;
			finished_ = true;

			return;
		}
	}
};

int main(int argc, char **argv)
{
	signal(SIGINT, &terminate);

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

		g.m_.add(c);
	}

	g.m_.fin_init();

	if (interactive)
	{
		cin.setf(ios_base::skipws);
	
		while (! g.finished_)
		{
			g.disp();
	
			cin >> c;
	
			g.move(c);
		}

		g.disp();
	}
	else
	{
		/*
		while (true)
		{
		}
		*/
	
		cout << "A" << endl;
	}

	return 0;
}

