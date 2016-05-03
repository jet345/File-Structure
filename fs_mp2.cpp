/* ----------------------------------------------------------------------- */
/* 2016-1 학기, 파일 구조 CP20333, 060 분반, 3 학점, 담당 교수, 김 경석 */
/* 숙제 1:  */
/* 학번:201324523              */
/* 이름:조경미                    */
/* 성적 게시용 딴 이름:                  */

/* ----------------------------------------------------------------------- */
/*
본인은 이 숙제 등을 하면서
다른 사람(온라인 자료 포함)의 것을 베끼지 않았으며,
다른 사람에게 주지도 않는 등
부산대의 "학술적 부정 행위 방지를 위한 지침"
(http://asadal.pnu.kr/~gimgs0/crs/gen-info/plag_sw_20151_1.pdf)에 따라
본인의 힘으로 직접 하였음을 확인합니다.

또한 표절 등 학술적 부정 행위를 한 때에는 벌칙이 있음을 잘 알고 있습니다.

날짜: 2016.03.28.
본인: 이름 조경미 (서명: 조경미)
* 전자 파일로 낼 때에는 서명 자리에 본인 이름을 한 번 더 적을 것)
*/

/* ----------------------------------------------------------------------- */

/* [중요] 각 문제를 풀었는지, 풀지 못 했는지 명확하게 나타낼 것. */
/* 풀었으면 OOO, 풀지 못 하였으면 XXX 로 나타낸다. */
/* 따라서 푼 문제에 대하여는 아래의 XXX 를 OOO 로 바꿀 것. */
/* 못 푼 문제에 대하여 OOO 로 나타내면 안 됨 ^-^ */


/* ----------------------------------------------------------------------- */
/* 2.0 위의 1에 나오는 여러 함수를 모두 짠다.
lfs_init (), lfs_cf (), lfs_cp (), lfs_hlink (), lfs_symlink (), lfs_display (), 그 밖에도 더 필요한 함수를 여기에 둔다. */


#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

struct directory {
	vector <string> file_name;
	vector <int> inode;
	vector <char> file_type;
};

struct IT {
	vector <int> inode;
	vector <int> ref_cnt;
	vector<int> file_size;
	vector<string> data_block;
	vector<char> file_type;

};

struct datablock {
	vector<string> content;
	vector<string> data_block;
	datablock *db_for_cd;
};

ofstream outf;

directory dir;
IT inode_table;
datablock db;



char* my_itoa(int value, char* a) {
	int cipher;//몇자리 정수인지 판단
	int rest;//나머지
	int tnp;
	tnp = value;
	for (int i = 1;; i++) {
		tnp = tnp / 10; //항상 10진수로 표현
		if (tnp <= 0) {
			cipher = i;
			break;
		}
	}
	a[cipher] = NULL; //맨끝자리 표현

	for (int i = 1; i <= cipher; i++) {
		rest = value % 10;
		value = value / 10;
		a[cipher - i] = rest + '0';
	}
	return a;
}

void lfs_init(int data_block) {
	char a[100];
	my_itoa(data_block, a);
	dir.file_name.push_back(".");
	dir.file_name.push_back("..");
	dir.file_type.push_back('d');//directory
	dir.file_type.push_back('d');
	dir.inode.push_back(2); // "."directory inode
	dir.inode.push_back(2);// ".." directory inode

	inode_table.inode.push_back(2);
	inode_table.ref_cnt.push_back(2);
	inode_table.file_size.push_back(1024);
	inode_table.data_block.push_back(a);
	inode_table.file_type.push_back('d');



}

void lfs_cf(string filename, int inode, int data_block, string contents) {
	//regular file 생성
	char a[100];
	my_itoa(data_block, a);
	dir.file_name.push_back(filename);
	dir.file_type.push_back('-');
	dir.inode.push_back(inode);

	inode_table.data_block.push_back(a);
	inode_table.ref_cnt.push_back(1);
	inode_table.file_type.push_back('-');
	inode_table.file_size.push_back(contents.length());
	inode_table.inode.push_back(inode);

	db.content.push_back(contents);
	db.data_block.push_back(a);

}

void lfs_cp(string source_file_name, string destination_file_name, int inode, int data_block) {
	//보기 lfs_cp("f11.txt","f12.txt",65,465)
	char a[100];
	my_itoa(data_block, a);
	int tnp = 0;
	int size;
	size = dir.file_name.size();
	string cp_string;
	for (int i = 0; i<size; i++) {

		if (source_file_name.compare(dir.file_name[i]) == 0) {
			tnp = i;
			cp_string = db.content[i - 2];
			break;
		}

	}

	dir.file_name.push_back(destination_file_name);
	dir.file_type.push_back('-');
	dir.inode.push_back(inode);

	inode_table.data_block.push_back(a);
	inode_table.ref_cnt.push_back(1);
	inode_table.file_type.push_back('-');
	inode_table.file_size.push_back(cp_string.length());
	inode_table.inode.push_back(inode);

	db.content.push_back(cp_string);
	db.data_block.push_back(a);

}

void lfs_hlink(string source_file_name, string destination_file_name) {
	//보기 lfs_hlink("f11.txt","f12.txt");
	int size;
	size = dir.file_name.size();


	int tnp = 0;

	for (int i = 0; i<size; i++) {

		if (source_file_name.compare(dir.file_name[i]) == 0) {
			tnp = i;
			break;
		}

	}
	dir.file_name.push_back(destination_file_name);
	dir.file_type.push_back(dir.file_type[tnp]);
	dir.inode.push_back(dir.inode[tnp]);

	inode_table.ref_cnt[tnp - 1]++;
}

void lfs_symlink(bool fastsym, string target_path, string soft_link_name, int inode, int data_block) {

	//fast symlink를 지원하지 않거나 target_path의 길이가 60이상이면 slow symlink를 만들고,
	//fast symlink를 지원하면서 target_path의 길이가 59이하이면 fast symlink를 만든다. 
	//fast symlink를 만들때는 data_block#은 무시한다(안쓴다)

	//보기 lfs_symlink(TRUE, "012345678901234567890123456789012345678901234567890123456789.txt","s33.txt",68,468) -----slow symlink생성
	//보기 lfs_symlink(TRUE, "f11.txt","s33.txt",68,468); -----fast symlink
	//보기 lfs_symlink(FALSE,"f11.txt","s34.txt",69,469); ----slow symlink
	char a[100];
	my_itoa(data_block, a);

	int tnp = 0;
	int size;
	size = dir.file_name.size();

	for (int i = 0; i<size; i++) {
		if (target_path.compare(dir.file_name[i]) == 0) {
			tnp = i;
			break;
		}
	}

	dir.file_name.push_back(soft_link_name);
	dir.file_type.push_back('l');
	dir.inode.push_back(inode);

	inode_table.inode.push_back(inode);
	inode_table.ref_cnt.push_back(1);
	inode_table.file_size.push_back(inode_table.file_size[tnp - 1]);
	inode_table.file_type.push_back('l');

	if (fastsym == true) {

		if (target_path.length() >= 60) {
			//fastsym을 지원하지만 targetpath가 60이상 slow symlink생성
			inode_table.data_block.push_back(a);

			db.data_block.push_back(a);
			db.content.push_back(target_path);
		}

		else {//fast symlink생성 
			inode_table.data_block.push_back(target_path);
		}
	}
	else {
		inode_table.data_block.push_back(a);

		db.data_block.push_back(a);
		db.content.push_back(target_path);
	}

}

void lfs_display(string state) {
	int size;
	size = inode_table.inode.size();
	int size2;
	size2 = dir.inode.size();
	int size3;
	size3 = db.data_block.size();

	outf << "state : " << state << endl;
	outf << endl;
	outf << endl;
	outf << "< Inode Table >" << endl;
	outf << "inode #         ref.cnt         data block #    file type       file size" << endl;
	for (int i = 0; i<size; i++) {
		outf << inode_table.inode[i] << "		";
		outf << inode_table.ref_cnt[i] << "		";
		outf << inode_table.data_block[i] << "		";
		outf << inode_table.file_type[i] << "		";
		outf << inode_table.file_size[i];
		outf << endl;

	}
	outf << endl;
	outf << "< Data Block # " << inode_table.data_block[0] << " >" << endl;
	outf << "file name       file type       inode #" << endl;


	for (int i = 0; i<size2; i++) {
		outf << dir.file_name[i] << "		";
		outf << dir.file_type[i] << "		";
		outf << dir.inode[i];
		outf << endl;
	}

	outf << endl;

	for (int i = 0; i<size3; i++) {
		outf << "< Data Block # " << db.data_block[i] << " >" << endl;
		outf << db.content[i] << endl << endl;
	}

	dir.file_name.clear();
	dir.file_type.clear();
	dir.inode.clear();
	inode_table.data_block.clear();
	inode_table.file_size.clear();
	inode_table.file_type.clear();
	inode_table.inode.clear();
	inode_table.ref_cnt.clear();
	db.content.clear();
	db.data_block.clear();

}

void lfs_cat(String filename) {
	int index = 0;
	String tnp;
	char a[100];

	for (int i = 0; i < dir.file_name.size(); i++) {
		if (filename.compare(dir.file_name[i]) == 0) {
			index = i;
			break;
		}
	}

	for (int i = 0; i < inode_table.inode.size(); i++) {
		if (dir.inode[index] == inode_table.inode[i]) {
			tnp = inode_table.data_block[i];
			break;
		}
	}

	for (int i = 0; i < db.data_block.size(); i++) {
		if(tnp.compare(db.data_block[i])==0){
			out << db.content[i] << endl;
			break;
	}
}

}

void lfs_mkdir(String dirname, int inode, int datablock) {
	char a[100];
	my_itoa(data_block, a);
	dir.file_name.push_back(dirname);
	dir.file_type.push_back('d');
	dir.inode.push_back(inode);

	inode_table.data_block.push_back(a);
	inode_table.ref_cnt.push_back(2);
	inode_table.ref_cnt[0]++;
	inode_table.file_type.push_back('d');
	inode_table.file_size.push_back(1024);
	inode_table.inode.push_back(inode);


}

void lfs_cd
/* ----------------------------------------------------------------------- */
/* 또한 여기에 state_s0(), state_cp1(), state_hl1(), state_ss1(), state_fs1() 등을 모두 여기에 둔다. */
void state_s0() {

	lfs_init(403);
	lfs_cf("f11.txt", 61, 461, "가나다라");
}

void state_cp1() {
	lfs_init(403);
	lfs_cf("f11.txt", 61, 461, "가나다라");
	lfs_cp("f11.txt", "f12.txt", 65, 465);

}

void state_hl1() {
	lfs_init(403);
	lfs_cf("f11.txt", 61, 461, "가나다라");
	lfs_hlink("f11.txt", "f22.txt");

}

void state_ss1() {
	lfs_init(403);
	lfs_cf("longfilenamelongfilenamelongfilenamelongfilenamelongfilename", 61, 461, "가나다라");
	lfs_symlink(true, "longfilenamelongfilenamelongfilenamelongfilenamelongfilename", "s33.txt", 68, 468);


}

void state_fs1() {
	lfs_init(403);
	lfs_cf("f11.txt", 61, 461, "가나다라");
	lfs_symlink(true, "f11.txt", "s34.txt", 69, 469);



}
/* ----------------------------------------------------------------------- */
int main() {
	outf.open("fs_mp1_out.txt");

	/* 2.1 상태 s0 만들기 XXX */

	state_s0();
	lfs_display("s0");


	/* ----------------------------------------------------------------------- */
	/* 2.2 상태 cp1 만들기 XXX */

	state_cp1();
	lfs_display("cp1");


	/* ----------------------------------------------------------------------- */
	/* 2.3 상태 hl1 만들기 XXX */

	state_hl1();
	lfs_display("hl1");


	/* ----------------------------------------------------------------------- */
	/* 2.4 상태 ss1 만들기 XXX */

	state_ss1();
	lfs_display("ss1");


	/* ----------------------------------------------------------------------- */
	/* 2.5 상태 fs1 만들기 XXX */

	state_fs1();
	lfs_display("fs1");
	/* ----------------------------------------------------------------------- */

}