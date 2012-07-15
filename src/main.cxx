
#undef DEV

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

time_t exec_start;

string sol;
string best_sol;
size_t best;
int best_sc;
char last_m;

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
	int water_;
	int flooding_;
	int waterproof_;
	int flcnt_, wpcnt_;
	map<char, pair<int, int> > trampolines_;
	map<char, pair<int, int> > tramp_tgts_;
	map<char, char> tlink_;
	map<char, list<char> > trevlink_;
	int grrt_;
	int razors_;
	map<pair<int, int>, int> beards_;
	int beards_shaven_;
	set<pair<int, int> > rlist_;
	int lrocks_;

	bd_map(): n_(0), m_(0), i_(0),
			r_x_(0), r_y_(0), l_x_(0), l_y_(0),
			ls_(0), last_n_(true),
			x_(vector<vector<char> >(1)),
			lambdae_(set<pair<int, int> >()),
			water_(0), flooding_(0), waterproof_(10),
			flcnt_(0), wpcnt_(10),
			trampolines_(map<char, pair<int, int> >()),
			tramp_tgts_(map<char, pair<int, int> >()),
			tlink_(map<char, char>()),
			trevlink_(map<char, list<char> >()),
			grrt_(25), razors_(0),
			beards_(map<pair<int, int>, int>()),
			beards_shaven_(0),
			lrocks_(0)
	{
		x_.push_back(vector<char>(0));
	}

	bool add(char c)
	{
		switch (c)
		{
			case '@':
				++lrocks_;
			case '\\':
				++ls_;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '#':
			case ' ':
			case '.':
			case '*':
			case 'R':
			case 'L':
			case 'W':
			case '!':
				if (c >= 'A' && c <= 'I')
				{
					trampolines_[c] = make_pair(i_, m_);
				}
				else if (c >= '1' && c <= '9')
				{
					tramp_tgts_[c] = make_pair(i_, m_);
				}
				else if (c == 'R')
				{
					r_x_ = i_;
					r_y_ = m_;
				}
				else if (c == 'L')
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

		for (map<char, pair<int, int> >::iterator iter = trampolines_.begin();
				iter != trampolines_.end(); ++iter)
		{
			iter->second.second = m_ - iter->second.second;
		}

		for (map<char, pair<int, int> >::iterator iter = tramp_tgts_.begin();
				iter != tramp_tgts_.end(); ++iter)
		{
			iter->second.second = m_ - iter->second.second;
		}

		for (int i = 0; i <= m_; ++i)
		{
			int cs = x_[m_ - i].size() - 1;

			for (int j = 0; j != n_ - cs; ++j)
			{
				x_[m_ - i].push_back(' ');
			}

			for (int j = 0; j <= n_; ++j)
			{
				if ((*this)(j, i) == '\\')
				{
					lambdae_.insert(make_pair(j, i));
				}
				else if ((*this)(j, i) == '!')
				{
					rlist_.insert(make_pair(j, i));
				}
				else if ((*this)(j, i) == 'W')
				{
					beards_[make_pair(j, i)] = grrt_ - 1;
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
		beards_shaven_ = 0;

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
			case 'S':
				return s();
			case 'A':
				return w();
			default:
				throw invalid_argument(string("Huh? - ") + c);
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

		char mv_tgt = (*this)(nx, ny);

		switch (mv_tgt)
		{
			case '\\':
				r = true;
			case ' ':
			case '.':
			case 'O':
			case '!':
				if (mv_tgt == '!')
				{
					++razors_;
				}
				ok = true;
				break;
			case '@':
			case '*':
				if (valid(nx - 1, ny) && (*this)(nx - 1, ny) == ' ')
				{
					(*this)(nx - 1, ny) = mv_tgt;
					ok = true;
				}
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
				(*this)(r_x_, r_y_) = ' ';
				r_x_ = nx; r_y_ = ny;
				(*this)(r_x_, r_y_) = ' ';
				char tr_tgt = tlink_[mv_tgt];
				int tr_x = tramp_tgts_[tr_tgt].first;
				int tr_y = tramp_tgts_[tr_tgt].second;
				r_x_ = tr_x; r_y_ = tr_y;
				(*this)(r_x_, r_y_) = 'R';
				for (list<char>::iterator iter = trevlink_[tr_tgt].begin();
						iter != trevlink_[tr_tgt].end(); ++iter)
				{
					(*this)(trampolines_[*iter].first, trampolines_[*iter].second) = ' ';
					trampolines_.erase(*iter);
				}
				return false;
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

		char mv_tgt = (*this)(nx, ny);

		switch (mv_tgt)
		{
			case '\\':
				r = true;
			case ' ':
			case '.':
			case 'O':
			case '!':
				if (mv_tgt == '!')
				{
					++razors_;
				}
				ok = true;
				break;
			case '@':
			case '*':
				if (valid(nx + 1, ny) && (*this)(nx + 1, ny) == ' ')
				{
					(*this)(nx + 1, ny) = mv_tgt;
					ok = true;
				}
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
				(*this)(r_x_, r_y_) = ' ';
				r_x_ = nx; r_y_ = ny;
				(*this)(r_x_, r_y_) = ' ';
				char tr_tgt = tlink_[mv_tgt];
				int tr_x = tramp_tgts_[tr_tgt].first;
				int tr_y = tramp_tgts_[tr_tgt].second;
				r_x_ = tr_x; r_y_ = tr_y;
				(*this)(r_x_, r_y_) = 'R';
				for (list<char>::iterator iter = trevlink_[tr_tgt].begin();
						iter != trevlink_[tr_tgt].end(); ++iter)
				{
					(*this)(trampolines_[*iter].first, trampolines_[*iter].second) = ' ';
					trampolines_.erase(*iter);
				}
				return false;
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

		char mv_tgt = (*this)(nx, ny);

		switch (mv_tgt)
		{
			case '\\':
				r = true;
			case ' ':
			case '.':
			case 'O':
			case '!':
				if (mv_tgt == '!')
				{
					++razors_;
				}
				ok = true;
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
				(*this)(r_x_, r_y_) = ' ';
				r_x_ = nx; r_y_ = ny;
				(*this)(r_x_, r_y_) = ' ';
				char tr_tgt = tlink_[mv_tgt];
				int tr_x = tramp_tgts_[tr_tgt].first;
				int tr_y = tramp_tgts_[tr_tgt].second;
				r_x_ = tr_x; r_y_ = tr_y;
				(*this)(r_x_, r_y_) = 'R';
				for (list<char>::iterator iter = trevlink_[tr_tgt].begin();
						iter != trevlink_[tr_tgt].end(); ++iter)
				{
					(*this)(trampolines_[*iter].first, trampolines_[*iter].second) = ' ';
					trampolines_.erase(*iter);
				}
				return false;
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

		char mv_tgt = (*this)(nx, ny);

		switch (mv_tgt)
		{
			case '\\':
				r = true;
			case ' ':
			case '.':
			case 'O':
			case '!':
				if (mv_tgt == '!')
				{
					++razors_;
				}
				ok = true;
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
				(*this)(r_x_, r_y_) = ' ';
				r_x_ = nx; r_y_ = ny;
				(*this)(r_x_, r_y_) = ' ';
				char tr_tgt = tlink_[mv_tgt];
				int tr_x = tramp_tgts_[tr_tgt].first;
				int tr_y = tramp_tgts_[tr_tgt].second;
				r_x_ = tr_x; r_y_ = tr_y;
				(*this)(r_x_, r_y_) = 'R';
				for (list<char>::iterator iter = trevlink_[tr_tgt].begin();
						iter != trevlink_[tr_tgt].end(); ++iter)
				{
					(*this)(trampolines_[*iter].first, trampolines_[*iter].second) = ' ';
					trampolines_.erase(*iter);
				}
				return false;
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

	bool s()
	{
		if (razors_ > 0)
		{
			--razors_;

			int i = r_y_, j = r_x_;

			if (valid(j - 1, i) && (*this)(j - 1, i) == 'W')
			{
				(*this)(j - 1, i) = ' ';
				++beards_shaven_;
			}
			if (valid(j, i - 1) && (*this)(j, i - 1) == 'W')
			{
				(*this)(j, i - 1) = ' ';
				++beards_shaven_;
			}
			if (valid(j + 1, i) && (*this)(j + 1, i) == 'W')
			{
				(*this)(j + 1, i) = ' ';
				++beards_shaven_;
			}
			if (valid(j, i + 1) && (*this)(j, i + 1) == 'W')
			{
				(*this)(j, i + 1) = ' ';
				++beards_shaven_;
			}
			if (valid(j - 1, i - 1) && (*this)(j - 1, i - 1) == 'W')
			{
				(*this)(j - 1, i - 1) = ' ';
				++beards_shaven_;
			}
			if (valid(j + 1, i + 1) && (*this)(j + 1, i + 1) == 'W')
			{
				(*this)(j + 1, i + 1) = ' ';
				++beards_shaven_;
			}
			if (valid(j - 1, i + 1) && (*this)(j - 1, i + 1) == 'W')
			{
				(*this)(j - 1, i + 1) = ' ';
				++beards_shaven_;
			}
			if (valid(j + 1, i - 1) && (*this)(j + 1, i - 1) == 'W')
			{
				(*this)(j + 1, i - 1) = ' ';
				++beards_shaven_;
			}
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

		// it's not necessarily a good idea, but we'll go actively looking for razors

		for (set<pair<int, int> >::iterator l_iter = rlist_.begin();
				l_iter != rlist_.end(); ++l_iter)
		{
			c_dist = abs(r_x_ - l_iter->first) + abs(r_y_ - l_iter->second);

			if (c_dist < r_dist)
			{
				r = make_pair(l_iter->first, l_iter->second);
				r_dist = c_dist;
			}
		}

		// this might be an even worse idea than the one above.

		/*
		for (map<char, pair<int, int> >::iterator l_iter = trampolines_.begin();
				l_iter != trampolines_.end(); ++l_iter)
		{
			c_dist = abs(r_x_ - l_iter->second.first) + abs(r_y_ - l_iter->second.second);

			if (c_dist < r_dist)
			{
				r = make_pair(l_iter->second.first, l_iter->second.second);
				r_dist = c_dist;
			}
		}
		*/

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
	int rocks_broken_;
	int rocks_moving_;
	deque<pair<int, int> > lmoves_;
	pair<int, int> beacon_of_unhope_;
	int bgauge_;

	bd_game(): finished_(false), escaped_(false), died_(false), aborted_(false),
			changed_(false),
			ls_(0), sc_(0), m_(bd_map()),
			rocks_broken_(0), rocks_moving_(0),
			lmoves_(deque<pair<int, int> >()),
			beacon_of_unhope_(pair<int, int>()),
			bgauge_(0)
	{
	}

	void disp()
	{
		pair<int, int> nl = m_.get_n_l();

		cout << "Score: " << sc_ << " Lambdas: " << ls_ << " (" << m_.ls_ << " remaining)" << endl;
		cout << "Target: (" << nl.first << ", " << nl.second << ")" << endl;
		cout << "Water level: " << m_.water_ << " Flooding counter: " << m_.flcnt_ << " WP counter: " << m_.wpcnt_ << endl;
		cout << "Razors: " << m_.razors_ << endl;
		m_.disp();

		if (escaped_)
		{
			cout << "ESCAPED!" << endl;
		}
		else if (aborted_)
		{
			cout << "ABORTED." << endl;
		}
		else if (died_)
		{
			cout << "DIED." << endl;
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
		lmoves_.push_back(make_pair(m_.r_x_, m_.r_y_));
		if (lmoves_.size() > 16)
		{
			lmoves_.pop_front();
		}
		if (bgauge_ > 0)
		{
			--bgauge_;
		}
		else if (lmoves_.size() == 16)
		{
			int mr_x = 0, mr_y = 0;
			for (deque<pair<int, int> >::iterator iter = lmoves_.begin();
					iter != lmoves_.end(); ++iter)
			{
				mr_x += iter->first;
				mr_y += iter->second;
			}
			mr_x /= 16;
			mr_y /= 16;
			int avg_d = 0;
			for (deque<pair<int, int> >::iterator iter = lmoves_.begin();
					iter != lmoves_.end(); ++iter)
			{
				avg_d += abs(mr_x - iter->first) + abs(mr_y - iter->second);
			}
			avg_d /= 16;
			if (avg_d <= 2)
			{
				beacon_of_unhope_ = make_pair(mr_x, mr_y);
				bgauge_ = 16;

#ifdef DEV
				//cout << "Setting the Beacon of Unhope at (" << mr_x << ", " << mr_y << ")" << endl;
#endif
			}
		}
		update();
		end(c);
	}

	void update()
	{
		bd_map nm = m_;

		changed_ = false;
		rocks_broken_ = 0;

		for (int i = 0; i <= m_.m_; ++i)
		{
			for (int j = 0; j <= m_.n_; ++j)
			{
				if (m_.valid(j, i - 1) && (m_(j, i) == '*' || m_(j, i) == '@') && m_(j, i - 1) == ' ')
				{
					nm(j, i) = ' ';
					nm(j, i - 1) = m_(j, i);
					if(m_(j, i) == '@')
					{
						++rocks_moving_;
					}
					if (m_(j, i) == '@' && m_.valid(j, i - 2) && m_(j, i - 2) != ' ')
					{
						nm(j, i - 1) = '\\';
						nm.lambdae_.insert(make_pair(j, i - 1));
						--nm.lrocks_;
						++rocks_broken_;
					}
					changed_ = true;
					if (j == m_.r_x_ && i - 1 == m_.r_y_ + 1)
					{
						died_ = true;
					}
				}
				else if (m_.valid(j, i - 1) && m_.valid(j + 1, i) && /* m_.valid(j + 1, i - 1) && */
						(m_(j, i) == '*' || m_(j, i) == '@') &&
						(m_(j, i - 1) == '*' || m_(j, i - 1) == '@') &&
						m_(j + 1, i) == ' ' && m_(j + 1, i - 1) == ' ')
				{
					nm(j, i) = ' ';
					nm(j + 1, i - 1) = m_(j, i);
					if(m_(j, i) == '@')
					{
						++rocks_moving_;
					}
					if (m_(j, i) == '@' && m_.valid(j + 1, i - 2) && m_(j + 1, i - 2) != ' ')
					{
						nm(j + 1, i - 1) = '\\';
						nm.lambdae_.insert(make_pair(j + 1, i - 1));
						--nm.lrocks_;
						++rocks_broken_;
					}
					changed_ = true;
					if (j + 1 == m_.r_x_ && i - 1 == m_.r_y_ + 1)
					{
						died_ = true;
					}
				}
				else if (m_.valid(j, i - 1) && m_.valid(j + 1, i) && m_.valid(j - 1, i) && /* some obviously true checks skipped */
						(m_(j, i) == '*' || m_(j, i) == '@') &&
						(m_(j, i - 1) == '*' || m_(j, i - 1) == '@') &&
						(m_(j + 1, i) != ' ' || m_(j + 1, i - 1) != ' ') && m_(j - 1, i) == ' ' && m_(j - 1, i - 1) == ' ')
				{
					nm(j, i) = ' ';
					nm(j - 1, i - 1) = m_(j, i);
					if(m_(j, i) == '@')
					{
						++rocks_moving_;
					}
					if (m_(j, i) == '@' && m_.valid(j - 1, i - 2) && m_(j - 1, i - 2) != ' ')
					{
						nm(j - 1, i - 1) = '\\';
						nm.lambdae_.insert(make_pair(j - 1, i - 1));
						--nm.lrocks_;
						++rocks_broken_;
					}
					changed_ = true;
					if (j - 1 == m_.r_x_ && i - 1 == m_.r_y_ + 1)
					{
						died_ = true;
					}
				}
				else if (m_.valid(j, i - 1) && m_.valid(j + 1, i) && /* m_.valid(j + 1, i - 1) && */
						(m_(j, i) == '*' || m_(j, i) == '@') &&
						m_(j, i - 1) == '\\' && m_(j + 1, i) == ' ' && m_(j + 1, i - 1) == ' ')
				{
					nm(j, i) = ' ';
					nm(j + 1, i - 1) = m_(j, i);
					if(m_(j, i) == '@')
					{
						++rocks_moving_;
					}
					if (m_(j, i) == '@' && m_.valid(j + 1, i - 2) && m_(j + 1, i - 2) != ' ')
					{
						nm(j + 1, i - 1) = '\\';
						nm.lambdae_.insert(make_pair(j + 1, i - 1));
						--nm.lrocks_;
						++rocks_broken_;
					}
					changed_ = true;
					if (j + 1 == m_.r_x_ && i - 1 == m_.r_y_ + 1)
					{
						died_ = true;
					}
				}
				else if (m_(j, i) == 'W')
				{
					if (m_.beards_[make_pair(j, i)] == 0)
					{
						nm.beards_[make_pair(j, i)] = nm.grrt_ - 1;
						if (nm.valid(j - 1, i) && m_(j - 1, i) == ' ')
						{
							nm(j - 1, i) = 'W';
							nm.beards_[make_pair(j - 1, i)] = nm.grrt_ - 1;
						}
						if (nm.valid(j + 1, i) && m_(j + 1, i) == ' ')
						{
							nm(j + 1, i) = 'W';
							nm.beards_[make_pair(j + 1, i)] = nm.grrt_ - 1;
						}
						if (nm.valid(j, i - 1) && m_(j, i - 1) == ' ')
						{
							nm(j, i - 1) = 'W';
							nm.beards_[make_pair(j, i - 1)] = nm.grrt_ - 1;
						}
						if (nm.valid(j, i + 1) && m_(j, i + 1) == ' ')
						{
							nm(j, i + 1) = 'W';
							nm.beards_[make_pair(j, i + 1)] = nm.grrt_ - 1;
						}
						if (nm.valid(j - 1, i - 1) && m_(j - 1, i - 1) == ' ')
						{
							nm(j - 1, i - 1) = 'W';
							nm.beards_[make_pair(j - 1, i - 1)] = nm.grrt_ - 1;
						}
						if (nm.valid(j + 1, i + 1) && m_(j + 1, i + 1) == ' ')
						{
							nm(j + 1, i + 1) = 'W';
							nm.beards_[make_pair(j + 1, i + 1)] = nm.grrt_ - 1;
						}
						if (nm.valid(j - 1, i + 1) && m_(j - 1, i + 1) == ' ')
						{
							nm(j - 1, i + 1) = 'W';
							nm.beards_[make_pair(j - 1, i + 1)] = nm.grrt_ - 1;
						}
						if (nm.valid(j + 1, i - 1) && m_(j + 1, i - 1) == ' ')
						{
							nm(j + 1, i - 1) = 'W';
							nm.beards_[make_pair(j + 1, i - 1)] = nm.grrt_ - 1;
						}
					}
					else
					{
						--nm.beards_[make_pair(j, i)];
					}
				}
			}
		}

		if (m_.ls_ == 0 && nm(m_.l_x_, m_.l_y_) != 'O')
		{
			nm(m_.l_x_, m_.l_y_) = 'O';
			changed_ = true;
		}

		if (nm.flooding_ > 0)
		{
			--nm.flcnt_;

			if (nm.flcnt_ == 0)
			{
				nm.flcnt_ = nm.flooding_;
				++nm.water_;
			}
		}

		if (m_.r_y_ > m_.water_)
		{
			nm.wpcnt_ = nm.waterproof_;
		}

		if (nm.r_y_ <= nm.water_)
		{
			--nm.wpcnt_;

			if (nm.wpcnt_ == 0)
			{
				died_ = true;
			}
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

	int get_b_sc()
	{
		if (finished_)
		{
			return sc_;
		}

		return sc_ + (ls_ * 25);
	}
};

int cost_func(bd_game &g1, bd_game &g2)
{
	if (g2.died_)
	{
		return -1000000;
	}

	if (g2.m_.r_x_ == g2.m_.l_x_ && g2.m_.r_y_ == g2.m_.l_y_)
	{
		return 100000;
	}

	int res = 0;

	// Without an additional incentive to shave, my Robo seems to go all Bender-like.
	res += g2.m_.beards_shaven_ * 25;

	if (g2.m_.razors_ > g1.m_.razors_)
	{
		res += 100;
	}

	res += g2.rocks_moving_ * 10;

	// Okay, let's give it a shot...
	if (g2.bgauge_ > 0)
	{
		res -=
				max(4 - abs(g2.m_.r_x_ - g2.beacon_of_unhope_.first), 0) +
				max(4 - abs(g2.m_.r_y_ - g2.beacon_of_unhope_.second), 0);
	}

	if (g2.m_.lrocks_ < g1.m_.lrocks_)
	{
		res += (g1.m_.lrocks_ - g2.m_.lrocks_) * 100;
	}

	if (g2.ls_ > g1.ls_)
	{
		return 50 + res;
	}

	if (g2.m_.get_c_dist() < g1.m_.get_c_dist())
	{
		return 3 + res;
	}

	if (g1.m_.r_x_ != g2.m_.r_x_ || g1.m_.r_y_ != g2.m_.r_y_ || g2.changed_)
	{
		return 0 + res;
	}

	return -1000000;
}

// Global:
// TODO: Glob best robo, backtrack if stuck.
// TODO: ACTUAL pathfinding. Duh.
// TODO: Prioritize targets.
// TODO: Regions?
// TODO: How the HELL do I avoid dropping rocks on important stuff? Reachability. Duh.
// Local:
// TODO: Tgt bottom lambdas in flooded areas?
// TODO (cont.): IDEA - favour staying low in flooded areas?
// TODO: Teach it to dive?
// TODO: Bummer. Lambda stones crashing into each other can make a map unsolvable.
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

		// 6 is too much, 4 probably too low.
		pair<char, int> mm = pick_a_move(5, last_m);

		m = mm.first;

		sol += m;

#ifdef DEV
		//cout << "Next move: " << m << endl;
#endif

		g_.move(m);

		if (m != 'W')
		{
			last_m = m;
		}

		if (g_.get_b_sc() > best_sc)
		{
			best_sc = g_.get_b_sc();
			best = sol.size();
			best_sol = sol;
		}
	}

	pair<char, int> pick_a_move(int lookahead, char last_m)
	{
		vector<char> res(0);
		res.push_back('A');
		int res_f = -1000;

		if (g_.get_b_sc() < best_sc - max(((g_.m_.m_ + g_.m_.n_) * 3), 256))
		{
			return make_pair('A', res_f);
		}

		char m[7] = "LRUDWS";

		bd_game g0;

		for (int i = 0; i < 6; ++i)
		{
			g0 = emulate(m[i]);

			if (i < 4 && g_.m_.r_x_ == g0.m_.r_x_ && g_.m_.r_y_ == g0.m_.r_y_)
			{
				continue;
			}

			if (i == 5 && g0.m_.beards_shaven_ == 0)
			{
				continue;
			}

			int f;

			// Do not penalize backtracking.
			// Surprisingly, inability to walk backwards is detrimental to a robot's navigational ken.
			f = cost_func(g_, g0) - (i == 4 ? 1 : 0) - (m[i] == last_m ? 0 : 0);

			if (f > -100000 && lookahead > 0)
			{
				bd_robo r0(g0);

				pair<char, int> m0 = r0.pick_a_move(lookahead - 1, i < 4 ? last_m : m[i]);

				// A BAD idea.
				f += (1 * m0.second) / 1;
			}
	
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

#ifdef DEV
		/*
		cout << res_f << " Moves: ";
		for (vector<char>::iterator iter = res.begin(); iter != res.end(); ++iter)
		{
			cout << (*iter);
		}
		cout << endl;
		*/
#endif

		int ix = rand() % res.size();

		return make_pair(res[ix], res_f);
	}
};

bd_game g;

void terminate(int signal)
{
	signal = signal;

#ifdef DEV
	g.disp();

	cout << sol << endl;
	cout << "Best score: " << best_sc << " Best pos: " << best << endl;
#endif

	cout << best_sol << endl;

	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	signal(SIGINT, &terminate);

	exec_start = time(NULL);

	srand(time(NULL));

	ifstream is, sis;

	if (argc > 1)
	{
		interactive = true;
		is.open(argv[1]);
	}
	else
	{
		interactive = false;
	}

	bool file_sol = false;

	if (argc > 2)
	{
		file_sol = true;
		sis.open(argv[2]);
		sis.unsetf(ios_base::skipws);
	}

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
			// read teh goddamn metadata.

			string meta;

			(interactive ? is : cin).setf(ios_base::skipws);

			while (true)
			{
				(interactive ? is : cin) >> meta;

				if ((interactive ? is : cin).eof())
				{
					break;
				}

				if (meta == "Water")
				{
					int w;

					(interactive ? is : cin) >> w;
					
					g.m_.water_ = w;

#ifdef DEV
					cout << "Water level set to " << w << endl;
#endif
				}
				else if (meta == "Flooding")
				{
					int f;

					(interactive ? is : cin) >> f;
					
					g.m_.flooding_ = f;
					g.m_.flcnt_ = f;

#ifdef DEV
					cout << "Flooding rate set to " << f << endl;
#endif
				}
				else if (meta == "Waterproof")
				{
					int w;

					(interactive ? is : cin) >> w;
					
					g.m_.waterproof_ = w;
					g.m_.wpcnt_ = w;

#ifdef DEV
					cout << "Waterproof counter set to " << w << endl;
#endif
				}
				else if (meta == "Trampoline")
				{
					char from, to;

					(interactive ? is : cin) >> from;
					(interactive ? is : cin) >> meta;
					(interactive ? is : cin) >> to;
					
					g.m_.tlink_[from] = to;
					g.m_.trevlink_[to].push_front(from);

#ifdef DEV
					cout << "Trampoline " << from << " linked to " << to << endl;
#endif
				}
				else if (meta == "Growth")
				{
					int r;

					(interactive ? is : cin) >> r;
					
					g.m_.grrt_ = r;

#ifdef DEV
					cout << "Wadler's Factor set to " << r << endl;
#endif
				}
				else if (meta == "Razors")
				{
					int r;

					(interactive ? is : cin) >> r;
					
					g.m_.razors_ = r;

#ifdef DEV
					cout << "Number of razors carried changed to " << r << endl;
#endif
				}
				else
				{
					throw invalid_argument(string("Bad metadata tag: ") + meta);
				}
			}

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
	
			if (file_sol)
			{
				sis >> c;

				if (sis.eof())
				{
					break;
				}
			}
			else
			{
				c = toupper(PressAnyKey(""));
	
				// roguelike controls
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
					case '.':
						c = 'W';
						break;
					case 'Q':
						c = 'A';
						break;
				}
			}
	
			if (c != 'L' && c != 'R' && c != 'U' && c != 'D' && c != 'W' && c != 'A' && c != 'S')
			{
				c = 'A';
			}

			g.move(c);
		}

		g.disp();
	}
	else
	{
		best = 0;
		best_sc = g.sc_;
		last_m = '0';

		while (! g.finished_)
		{
			bd_robo r(g);

			try
			{
				r.solve();
			}
			catch (bad_alloc e)
			{
				break;
			}
		}

#ifdef DEV
		g.disp();

		cout << sol << endl;
		cout << "Best score: " << best_sc << " Best pos: " << best << endl;
#endif

		cout << best_sol << endl;
	}

	return 0;
}

