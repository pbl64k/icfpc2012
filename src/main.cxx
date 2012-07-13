
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
		if (x < 0 || x > n_ || y < 0 || y > m_)
		{
			throw out_of_range("D'oh.");
		}

		return x_[m_ - y][x];
	}

	void move(char c)
	{
		switch (c)
		{
			case 'L':
				l();
				break;
			case 'R':
				r();
				break;
			case 'U':
				u();
				break;
			case 'D':
				d();
				break;
			case 'W':
				break;
			case 'A':
				break;
			default:
				throw invalid_argument("Huh?");
		}
	}

	void l()
	{
	}

	void r()
	{
	}

	void u()
	{
	}

	void d()
	{
	}
};

class bd_game
{
	public:
	bool finished_;
	bool aborted_;
	int sc_;
	bd_map m_;

	bd_game(): finished_(false), aborted_(true),
			sc_(0), m_(bd_map())
	{
	}

	void disp()
	{
		cout << "Score: " << sc_ << endl;
		m_.disp();
	}

	void move(char c)
	{
		m_.move(c);
		update();
		end(c);
	}

	void update()
	{
	}

	void end(char c)
	{
		if (c == 'A')
		{
			aborted_ = true;
			finished_ = true;
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

