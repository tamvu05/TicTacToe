// Nguoi du lich

#include <iostream>
using namespace std;

int n, a[105][105], X[105], d = 0, ans = INT_MAX, amin = INT_MAX;
bool visited[105];

void Try(int i) {
	for(int j=1; j<=n; j++)
		if (visited[j] == 0) {
			visited[j] = 1;
			X[i] = j;
			d += a[X[i - 1]][X[i]];
			if (i == n) ans = min(ans, d + a[X[n]][1]);
			else if (d + (n - i + 1) * amin < ans) Try(i + 1);
			visited[j] = 0;
			d -= a[X[i - 1]][X[i]];
		}
}

int main() {
	cin >> n;
	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= n; j++) {
			cin >> a[i][j];
			if (a[i][j] != 0) amin = min(amin, a[i][j]);
		}
	X[1] = 1;
	visited[1] = 1;
	Try(2);
	cout << ans;

}

/**
11
0 7 83 7 98 95 96 43 19 5 77
7 0 90 91 91 93 85 47 88 29 24
83 90 0 95 44 12 58 32 78 20 51 
7 91 95 0 9 51 45 52 47 49 12
98 91 44 9 0 48 28 18 57 17 67
95 93 12 51 48 0 54 82 40 33 78 
96 85 58 45 28 54 0 55 31 22 100
43 47 32 52 18 82 55 0 66 97 76
19 88 78 47 57 40 31 66 0 58 68 
5 29 20 49 17 33 22 97 58 0 23
77 24 51 12 67 78 100 76 68 23 0
*/