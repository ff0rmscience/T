
#include<cstdio>
#include<iostream>
#include<cstdlib>
#include<set>
enum {
    K = 14, //key size
    T = 20
};

const char* a = "abcdefghijklmnopqrstuvwxyz_1234567890 !,;()*@#$^&";

struct T_system {
    T_system() {
        init_key();
		install_I();
		install_X();
    }
    std::set<std::string> F {};
    int k[K][K] {};
    int s[K] {}, f[K] {}, I[K] {}, zeros[K] {};
    int p[T] {}, c[T] {}, w[T] {},u[T] {}, X[T];
	//int tl = 40;//text length
    int r = 1;//rounds
    int autospin_factor = 0;
    int symbols = 0;
    bool details = true;
    bool reverse_mode = false;
	bool fancy = true;
	//key functionality
	void init_key() {
        for (int i = 0; i < K; i++) {
            id_perm(k[i]);
            shuffle(k[i]);
        }
    }
	void install_I() {
		for (int i = 0; i < K; i++) I[i] = i;
	}
	void install_X() {
		for (int i = 0; i < T; i++) X[i] = i;
	}
    void id_perm(int f[K]) {
        for (auto i : I) f[i] = i;
    }
    void shuffle(int f[K]) {
        int m,n;
        for (int i = K - 1; i > 0; i--) {
            m = random()%(i + 1);
            n = f[i];
            f[i] = f[m];
            f[m] = n;
        }
    }
    void print_key() {
        for (auto i : I) {
            for (auto j : I) {
                printf("%c", a[k[i][j]]);
            }
            printf("\n");
         }
        printf("\n");
    }
	void to_s_add(int r[K]) {
        for (auto i : I) s[i] = (s[i] + r[i]) % K;
    }
	void fill_s_with_zeros() {
		for (auto& i : s) i = 0;
	}
	void compute_f() {
        int x = 0;
        int d = 0;
        for (int i = 0; i < K; i++) {
            x = i;
            for (int j = 0; j < K; j++) {
                d = (s[j] + x) % K;
                x = k[j][d];
            }
        f[i] = x;
        }
    }
    std::string f_string() {
        std::string s = "";
        for (int i = 0; i < K; i++) {
            s += a[ f[i] ];
        }
        return s;
	}
	void encrypt_one_round() {
		//fill_s_with_zeros();
        for (auto i : X) {
            compute_f();
			if (fancy) F.insert(f_string());
            c[i] = f[c[i]];
            symbols++;
            to_s_add(k[c[i]]);
        }
    }
	void encrypt() {
        symbols = 0;
        if (fancy) F.clear();
        copy_text(p,c);
        for (int i = 0; i < r; i++) {
            auto_spin(i);
            encrypt_one_round();
            if (reverse_mode) reverse(c);
            if (details) {
                printf("encrypt %03d     ",i);
                print_(c);
            }
        }
        if (fancy) printf("used %lu unique permutations to process %d symbols\n", F.size(), symbols);
    }
	void decrypt_one_round() {
        int d = 0;
		//fill_s_with_zeros();
        for (auto i : X) {
            compute_f();
			if (fancy) F.insert(f_string());
            for (d = 0; f[d] != u[i]; d++);
            to_s_add(k[u[i]]);
            u[i] = d;
            symbols++;
        }
    }
    void decrypt() {
        symbols = 0;
        if (fancy) F.clear();
        copy_text(c,u);
        for (int i = 0; i < r ; i++) {
            auto_spin(r - 1 - i);
            if (reverse_mode) reverse(c);
            decrypt_one_round();
            if (details) {
                printf("decrypt %03d     ",i);
                print_(u);
            }
        }
        if (fancy) printf("%lu unique perm for %d symbols\n",F.size(), symbols);
    }
	void auto_spin(int n) {
        fill_s_with_zeros();
        for (int i = 0; i < n*autospin_factor; i++) {
            compute_f();
			to_s_add(f);
        }
    }
	void reverse(int q[T]) {
        int x[T] {};
        for (auto i : X)  x[i] = q[i];
        for (auto i : X) q[i] = x[T - 1 - i];
    }
	void copy_text(int source[K], int dest[K]) {
        for (auto i : X) dest[i] = source[i];
    }
	void print_(int text[T]) {
		for (auto i : X) printf("%c", a[text[i]]);
		printf("\n");
	}
	void text_color(int r, int g, int b) {
        printf("\033[38;2;%d;%d;%dm",r,g,b);
    }
	void randomize_(int p[T]) {
		for (auto i : X) p[i] = rand()%K;
	}
	int success() {
        for (auto i : X) {
            if (p[i] != u[i]) return i;
        }
        return -1;
    }

	void examples(int n) {
		init_key();
		print_key();
        for (int i = 0 ; i < n; i++) {
			randomize_(p);
            text_color(255,255,0);
            printf("plaintxt  =     ");
            print_(p);
            text_color(200,200,200);
            encrypt();
            //e_round();
            text_color(255,255,0);
            printf("ciphertxt =     ");
            print_(c);
            text_color(200,200,200);
            decrypt();
            //d_round();
            text_color(255,255,0);
            printf("plaintxt  =     ");
            print_(u);
            //print_(p);
            text_color(255,255,255);
            printf("failure at symbol %d\n", success());
            printf("\n\n");
        }
        fill_s_with_zeros();
    }
};

int main() {
	T_system S;
	S.r = 10;
	S.autospin_factor = 2;
	S.examples(2);
}
