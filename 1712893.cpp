#include <fcntl.h> //_O_U16TEXT
#include <io.h>    //_setmode()
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
const int truongsinhvien = 8;
#pragma pack(1)
typedef struct {
	const wchar_t* til = L"<title>";
	int indexoftil = 0;
	const wchar_t* fullname = L"Personal_FullName";
	int indexofname = 0;
	const wchar_t* Pdepartment = L"Personal_Department";
	int indexofPde = 0;
	const wchar_t *contact = L"Personal_Phone";
	int indexofcontact = 0;
	const wchar_t *picture = L"Personal_HinhcanhanKhung";
	int indexofpic = 0;
	const wchar_t *gioithieu = L"TextInList";
	int indexofintro1 = 0;
	int indexofintro2 = 0;
	const wchar_t *mota = L"Description";
	int indexofDes = 0;
	const wchar_t *dev = L"Tên sinh viên thực hiện";
	int indexofdev = 0;
}danhmuc;
typedef struct {
	wchar_t MSSV[11];
	wchar_t Hovaten[31];
	wchar_t Khoa[31];
	//wchar_t email[31];
	int NienKhoa;
	wchar_t Ngaysinh[11];
	wchar_t HinhanhcaNhan[100];
	wchar_t mota[1001];
	wchar_t sothich[1001];
}sinhvien;
void * taomang(int sophantu, int dolonmoiphantu) {//loi thi tra ve NULL
	if (sophantu < 0 || dolonmoiphantu <= 0)return NULL;
	void * k = calloc(1, sophantu*dolonmoiphantu + sizeof(int));
	if (k == NULL)return NULL;
	*(int*)k = sophantu;
	return (char*)k + sizeof(int);
}
int getsizearr(void* a) {//NULL thi tra ve -1
	if (a != NULL) return *(int*)((char*)a - sizeof(int));
	else return -1;
}
void * resizearr(void *arr, int sophantumoi, int dolonmoiphantu) {//khong cap phat duoc hoac loi thi tra ve NULL
	if (sophantumoi < 0 || arr == NULL || dolonmoiphantu <= 0)return NULL;
	if (sophantumoi == 0)return taomang(sophantumoi, dolonmoiphantu);
	void * buff = (char*)arr - sizeof(int);
	buff = realloc(buff, sizeof(int) + sophantumoi*dolonmoiphantu);
	if (buff == NULL)return NULL;
	*(int *)buff = sophantumoi;
	return (char*)buff + sizeof(int);
}
void pushbackkieusinhvien(void * a, void *b) {
	sinhvien * sv1 = (sinhvien*)a;
	sinhvien * sv2 = (sinhvien*)b;
	*sv1 = *sv2;
}
void pushbackkieuwchar_sao(void*a, void*b) {//a la con tro , b nhan bien dia chi
	void** z = (void**)a;
	*z = b;

}
void pushback(void *arr, void *data, int dolonmoiphantu, void(*howtopushback)(void*, void*) = NULL) {//arr la NULL thi khong lam gi ca hoac do lon nho hon 0
	void ** z = (void**)arr;
	if (*z == NULL || dolonmoiphantu <= 0) return;
	else {
		int oldlength = getsizearr(*z);
		*z = resizearr(*z, oldlength + 1, dolonmoiphantu);
		if (*z != NULL) {
			if (howtopushback == NULL)memmove((char*)*z + dolonmoiphantu*oldlength, data, dolonmoiphantu);
			else howtopushback((char*)*z + dolonmoiphantu*oldlength, data);
		}//a[old_length]=data
	}

}
void xoamangsinhvien(sinhvien * a) {
	if (a != NULL) {
		if (((char*)a - sizeof(int) != NULL))free((char*)a - sizeof(int));
	}
}
void xoamangbatki(void *a) {
	if (a != NULL) {
		if (((char*)a - sizeof(int) != NULL)) free((char*)a - sizeof(int));
	}
}
sinhvien* readDatafromcsv(wchar_t * filename) {
	FILE *fp = _wfopen(filename, L"r, ccs=UTF-8");
	if (fp == NULL)return NULL;
	else {
		sinhvien*k = (sinhvien*)taomang(0, sizeof(sinhvien));
		while (!feof(fp)) {
			sinhvien a;
			fwscanf(fp, L"%[^,],%[^,],%[^,]", &a.MSSV, &a.Hovaten, &a.Khoa);
			fseek(fp, 1, SEEK_CUR);
			fwscanf(fp, L"%d", &a.NienKhoa);
			fseek(fp, 1, SEEK_CUR);
			fwscanf(fp, L"%[^,],%[^,],%[^,]", &a.Ngaysinh, &a.HinhanhcaNhan, &a.mota);
			fseek(fp, 1, SEEK_CUR);
			fgetws(a.sothich, 1000, fp);
			if (a.sothich[0] == 34 || a.sothich[0] == 44) {//loai bo cac dau du thua
				int z = wcslen(a.sothich);
				int flag = 0;
				for (int i = 0; i < z; i++) {
					if (a.sothich[i] == 34 || (a.sothich[i] == 44 && flag == 0)) {
						flag = 1;
						for (int j = i; j < z; j++) {
							a.sothich[j] = a.sothich[j + 1];
						}
					}
				}
			}
			int z = wcslen(a.sothich);
			if (z == 1)a.sothich[z - 1] = '\0';
			if (z >= 2) {
				a.sothich[z - 1] = '\0';
				if (a.sothich[z - 2] == 44) a.sothich[z - 2] = '\0';
			}
			pushback(&k, &a, sizeof(sinhvien), pushbackkieusinhvien);// doc du sinh vien cuoi 2 lan ?? :D ?? 

		}
		k = (sinhvien*)resizearr(k, getsizearr(k) - 1, sizeof(sinhvien));// no chay toi ra 11 phan tu trong khi chi co 10 ?? :D ??
		fclose(fp);
		return k;

	}
}
wchar_t** readHTML(wchar_t* filename) {
	FILE *fp = _wfopen(filename, L"r, ccs=UTF-8");
	if (fp == NULL)return NULL;
	wchar_t ** mang = (wchar_t**)taomang(0, sizeof(wchar_t*));
	if (mang == NULL)return NULL;
	while (!feof(fp)) {
		wchar_t *s = (wchar_t*)calloc(sizeof(wchar_t) * 1002, 1);

		fgetws(s, 1002, fp);
		pushback(&mang, s, sizeof(wchar_t*), pushbackkieuwchar_sao);
		//free(s);
		// wprintf_s(mang[0]);
	}
	// wprintf_s(L"%d",getsizearr(mang));
	fclose(fp);
	return mang;
}
void xoamanghtml(wchar_t** z) {
	if (z != NULL) {
		int n = getsizearr(z);
		for (int i = 0; i < n; i++) { if (*(wchar_t**)((char*)z + i * sizeof(wchar_t*)) != NULL)free(*(wchar_t**)((char*)z + i * sizeof(wchar_t*))); }
		//int * testkey = (int*)((char*)z - sizeof(int));
		//int a = *testkey;
		if ((char*)z - sizeof(int) != NULL)free((char*)z - sizeof(int));
	}
}
danhmuc demchiso(wchar_t** arrhtml) {
	danhmuc a;
	int length = getsizearr(arrhtml);
	int flag = 0;
	for (int i = 0; i < length; i++) {
		if (wcsstr(arrhtml[i], a.til) != NULL) a.indexoftil = i;
		if (wcsstr(arrhtml[i], a.fullname) != NULL) a.indexofname = i;
		if (wcsstr(arrhtml[i], a.Pdepartment) != NULL)a.indexofPde = i;
		if (wcsstr(arrhtml[i], a.contact) != NULL)a.indexofcontact = i;
		if (wcsstr(arrhtml[i], a.picture) != NULL)a.indexofpic = i;
		if (wcsstr(arrhtml[i], a.gioithieu) != NULL) {
			if (flag == 0) {
				flag = 1;
				a.indexofintro1 = i;
			}
			else a.indexofintro2 = i;
		}
		if (wcsstr(arrhtml[i], a.dev) != NULL)a.indexofdev = i;
		if (wcsstr(arrhtml[i], a.mota) != NULL)a.indexofDes = i;
	}
	return a;
}
void replace(wchar_t * str, wchar_t *pat, int length) {
	if (length < 0 || str == NULL || pat == NULL)return;
	int oldlength = wcslen(str);
	if (length > oldlength)return;
	int newlength = wcslen(pat);
	memmove(str + newlength, str + length, sizeof(wchar_t)*(oldlength - length + 1));
	memmove(str, pat, newlength * sizeof(wchar_t));

}
void xuly1sinhvien(wchar_t** manghtml, sinhvien sv, int *a, danhmuc z) {
	if (a[0] == 0)wcscpy(sv.MSSV, L"");
	if (a[1] == 0)wcscpy(sv.Hovaten, L"");
	if (a[2] == 0)wcscpy(sv.Khoa, L"");
	//if (a[3] == 0)wcscpy(sv.email, L"");
	if (a[3] == 0)sv.NienKhoa = -1;
	if (a[4] == 0)wcscpy(sv.Ngaysinh, L"");
	if (a[5] == 0)wcscpy(sv.HinhanhcaNhan, L"");
	if (a[6] == 0)wcscpy(sv.mota, L"");
	if (a[7] == 0)wcscpy(sv.sothich, L"");
	if (1) {
		int pos1 = 0, pos2 = 0;
		wchar_t *y1 = wcschr(manghtml[z.indexoftil], L'-');
		pos1 = y1 - manghtml[z.indexoftil] + 2;
		y1 = wcschr(manghtml[z.indexoftil] + pos1 - 2, L'<');
		pos2 = y1 - manghtml[z.indexoftil];
		int length = pos2 - pos1;
		replace(manghtml[z.indexoftil] + pos1, sv.Hovaten, length);
	}
	if (1) {
		int pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
		wchar_t *y1 = wcschr(manghtml[z.indexofname], L'>');
		pos1 = y1 - manghtml[z.indexofname] + 1;
		y1 = wcschr(manghtml[z.indexofname] + pos1 - 1, L'-');
		pos2 = y1 - manghtml[z.indexofname] - 1;
		pos3 = pos2 + 3;
		y1 = wcschr(manghtml[z.indexofname] + pos2, L'<');
		pos4 = y1 - manghtml[z.indexofname];

		int length = pos2 - pos1;
		int length2 = pos4 - pos3;
		replace(manghtml[z.indexofname] + pos3, sv.MSSV, length2);
		replace(manghtml[z.indexofname] + pos1, sv.Hovaten, length);

	}
	if (1) {
		int pos1 = 0, pos2 = 0;
		wchar_t *y1 = wcschr(manghtml[z.indexofPde], L'>');
		pos1 = y1 - manghtml[z.indexofPde] + 1;
		y1 = wcschr(manghtml[z.indexofPde] + pos1 - 1, L'<');
		pos2 = y1 - manghtml[z.indexofPde];
		int length = pos2 - pos1;
		replace(manghtml[z.indexofPde] + pos1, sv.Khoa, length);
	}
	if (1) {
		int pos1 = 0, pos2 = 0;
		wchar_t *y1 = wcschr(manghtml[z.indexofcontact + 1], L':');
		pos1 = y1 - manghtml[z.indexofcontact + 1] + 1;
		y1 = wcschr(manghtml[z.indexofcontact + 1] + pos1 - 1, L'\n');
		pos2 = y1 - manghtml[z.indexofcontact + 1];
		int length = pos2 - pos1;
		wchar_t u[2] = L"\n";
		replace(manghtml[z.indexofcontact + 1] + pos1, u, length);
	}
	if (1) {
		int pos1 = 0, pos2 = 0;
		wchar_t *y1 = wcschr(manghtml[z.indexofpic + 1], L'"');
		pos1 = y1 - manghtml[z.indexofpic + 1] + 1;
		y1 = wcschr(manghtml[z.indexofpic + 1] + pos1, L'"');
		pos2 = y1 - manghtml[z.indexofpic + 1];
		int length = pos2 - pos1;
		replace(manghtml[z.indexofpic + 1] + pos1, sv.HinhanhcaNhan, length);
	}
	if (1) {
		for (int i = 1; i <= 5; i++) {
			int pos1 = 0, pos2 = 0;
			wchar_t a[255] = L"";
			if (i == 1) wcscpy(a, sv.Hovaten);
			if (i == 2) wcscpy(a, sv.MSSV);
			if (i == 3) wcscpy(a, sv.Khoa);
			if (i == 4) wcscpy(a, sv.Ngaysinh);
			//if (i == 5) wcscpy(a, sv.email);
			if (i != 3) {
				wchar_t *y1 = wcschr(manghtml[z.indexofintro1 + i], L':');
				pos1 = y1 - manghtml[z.indexofintro1 + i] + 2;
				y1 = wcschr(manghtml[z.indexofintro1 + i] + pos1 - 1, L'<');
				pos2 = y1 - manghtml[z.indexofintro1 + i];
				int length = pos2 - pos1;
				replace(manghtml[z.indexofintro1 + i] + pos1, a, length);
			}
			else {
				wchar_t *y1 = wcschr(manghtml[z.indexofintro1 + i], L' ');
				for (int j = 1; j <= 2; j++)y1 = wcschr(y1 + 1, L' ');
				pos1 = y1 - manghtml[z.indexofintro1 + i] + 1;
				y1 = wcschr(manghtml[z.indexofintro1 + i] + pos1 - 1, L'<');
				pos2 = y1 - manghtml[z.indexofintro1 + i];
				int length = pos2 - pos1;
				if (i == 5)wcscpy(a, L"\n");
				replace(manghtml[z.indexofintro1 + i] + pos1, a, length);
			}

		}
		for (int i = 1; i <= 2; i++) {
			wchar_t a[255] = { 0 };
			if (i == 1) {
				wcscpy(a, sv.sothich);
				//if (i == 2) wcscpy(a, sv.sothich);
				int pos1 = 0, pos2 = 0;
				wchar_t *y1 = wcschr(manghtml[z.indexofintro2 + i], L'>');
				pos1 = y1 - manghtml[z.indexofintro2 + i] + 1;
				y1 = wcschr(manghtml[z.indexofintro2 + i] + pos1, L'<');
				pos2 = y1 - manghtml[z.indexofintro2 + i];
				int length = pos2 - pos1;
				replace(manghtml[z.indexofintro2 + i] + pos1, a, length);
			}
			if (i == 2)wcscpy(manghtml[z.indexofintro2 + i], L"\n");

		}
	}
	if (1) {//ham replace co van de trong trg hop nay
		int pos1 = 0, pos2 = 0;
		wchar_t *y1 = wcschr(manghtml[z.indexofDes + 1], L' ');
		int i = 0;
		while (manghtml[z.indexofDes + 1][i] == L' ' || manghtml[z.indexofDes + 1][i] == L'\t')i++;
		pos1 = i;
		y1 = wcschr(manghtml[z.indexofDes + 1] + pos1 + 1, L'.');
		pos2 = y1 - manghtml[z.indexofDes + 1];
		int length = pos2 - pos1;
		replace(manghtml[z.indexofDes + 1] + pos1, sv.mota, length);
	}
	if (1) {
		int pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0, i = 0;
		wchar_t *y1 = wcschr(manghtml[z.indexofdev], L' ');
		while (manghtml[z.indexofdev][i] == L'\t' || manghtml[z.indexofdev][i] == L' ')i++;
		pos1 = i;
		y1 = wcschr(manghtml[z.indexofdev] + pos1, L'-');
		pos2 = y1 - manghtml[z.indexofdev] - 1;
		pos3 = pos2 + 3;
		y1 = wcschr(manghtml[z.indexofdev], L'<');
		pos4 = y1 - manghtml[z.indexofdev];
		int length1 = pos2 - pos1;
		int length2 = pos4 - pos3;
		replace(manghtml[z.indexofdev] + pos3, L"Trần Diệu Uy", length2);
		replace(manghtml[z.indexofdev] + pos1, L"1712893", length1);
	}
}
void menu(wchar_t* htmlfilename, sinhvien* mangsinhvien, danhmuc z) {
	int a[truongsinhvien] = { 1,1,1,1,1,1,1,1 };
	while (1) {
		wchar_t** manghtml = readHTML(htmlfilename);//do trong quá trình đọc thì nếu chọn xóa phần mô tả thì tại dòng mô tả sẽ là dòng trắng nên replace bị lỗi và không có dữ liệu gốc để cho các lần viết sau ==> đọc lại file html.(cách khác là tạo 1 mảng html tạm).
		if (manghtml == NULL)return;
		system("cls");
		wprintf(L"Nhập sai lựa chọn mặc định là thoát\n");
		wprintf(L"1. Tạo tất cả các sinh viên thành page\n");
		wprintf(L"2. Tạo sinh viên cụ thể và tạo tất cả các thông tin của sinh viên vào page\n");
		wprintf(L"3. Tùy chọn sinh viên cụ thể và tùy chọn các mục của sinh viên muốn nhập\n");
		wprintf(L"4. Tùy chọn thêm email và số điện thoại\n");
		wprintf(L"5. Thêm sở thích\n");
		wprintf(L"0. Thoát chương trình\n");
		int n;
		wprintf(L"Mời bạn nhập lựa chọn: ");
		wscanf(L"%d", &n);
		if (n > 5 || n < 0)n = 0;
		if (n == 0)break;
		if (n == 1) {
			int soluongsinhvien = getsizearr(mangsinhvien);
			int sodonghtml = getsizearr(manghtml);
			for (int i = 0; i < soluongsinhvien; i++)
			{
				xuly1sinhvien(manghtml, mangsinhvien[i], a, z);
				wchar_t link[255] = { L"../../../" };
				wcscat(link, mangsinhvien[i].MSSV);
				FILE *fp = _wfopen(wcscat(link, L".htm"), L"w, ccs=UTF-8");
				for (int j = 0; j < sodonghtml; j++)fputws(manghtml[j], fp);
				fclose(fp);
			}
		}
		if (n == 2) {
			int soluongsinhvien = getsizearr(mangsinhvien);
			for (int i = 0; i < soluongsinhvien; i++) {
				wprintf(L"%d) %ls,%ls,%ls,%d,%ls,%ls,%ls,%ls\n", i, mangsinhvien[i].MSSV, mangsinhvien[i].Hovaten, mangsinhvien[i].Khoa, mangsinhvien[i].NienKhoa, mangsinhvien[i].Ngaysinh, mangsinhvien[i].HinhanhcaNhan, mangsinhvien[i].mota, mangsinhvien[i].sothich);
			}
			int index;
			wprintf(L"Mời bạn nhập chỉ số sinh viên(nhập sai thì mặc định là 0): ");
			wscanf(L"%d", &index);
			if (index >= soluongsinhvien || index < 0)index = 0;
			int sodonghtml = getsizearr(manghtml);
			xuly1sinhvien(manghtml, mangsinhvien[index], a, z);
			wchar_t link[255] = { L"../../../" };
			wcscat(link, mangsinhvien[index].MSSV);
			FILE *fp = _wfopen(wcscat(link, L".htm"), L"w, ccs=UTF-8");
			for (int j = 0; j < sodonghtml; j++)fputws(manghtml[j], fp);
			fclose(fp);
		}
		if (n == 3) {
			int soluongsinhvien = getsizearr(mangsinhvien);
			int sodonghtml = getsizearr(manghtml);
			int index1;
			while (1) {
				wprintf(L"chọn những mục không muốn nhập: \n");
				wprintf(L"0) Mã số sinh viên\n");
				wprintf(L"1) Họ và tên\n");
				wprintf(L"2) Khoa\n");
				//wprintf(L"3) Email\n");
				wprintf(L"3) Khóa\n");
				wprintf(L"4) Ngày sinh\n");
				wprintf(L"5) Hình cá nhân\n");
				wprintf(L"6) Mô tả\n");
				wprintf(L"7) Sở thích\n");
				wprintf(L"8) Ngưng lựa chọn và bắt đầu xuất page\n");
				wprintf(L"Mời bạn nhập lựa chọn:");
				wscanf(L"%d", &index1);
				if (index1 == 8) break;
				if (index1 >= truongsinhvien || index1 < 0)index1 = 0;
				a[index1] = 0;
				system("cls");
			}
			int index;
			wprintf(L"Mời bạn nhập chỉ số sinh viên(nhập sai thì mặc định là 0): ");
			wscanf(L"%d", &index);
			if (index >= soluongsinhvien || index < 0)index = 0;
			sinhvien temp = mangsinhvien[index];
			xuly1sinhvien(manghtml, temp, a, z);
			wchar_t link[255] = { L"../../../" };
			wcscat(link, mangsinhvien[index].MSSV);
			FILE *fp = _wfopen(wcscat(link, L".htm"), L"w, ccs=UTF-8");
			for (int j = 0; j < sodonghtml; j++)fputws(manghtml[j], fp);
			fclose(fp);
		}
		if (n == 4) {
			danhmuc chisomoi;
			int soluongsinhvien = getsizearr(mangsinhvien);
			for (int i = 0; i < soluongsinhvien; i++) {
				wprintf(L"%d) %ls,%ls,%ls,%d,%ls,%ls,%ls,%ls\n", i, mangsinhvien[i].MSSV, mangsinhvien[i].Hovaten, mangsinhvien[i].Khoa, mangsinhvien[i].NienKhoa, mangsinhvien[i].Ngaysinh, mangsinhvien[i].HinhanhcaNhan, mangsinhvien[i].mota, mangsinhvien[i].sothich);
			}
			int index;
			wprintf(L"Mời bạn nhập chỉ số sinh viên(nhập sai thì mặc định là 0): ");
			wscanf(L"%d", &index);
			if (index >= soluongsinhvien || index < 0)index = 0;
			wchar_t duongdan[100] = { 0 };
			wcscpy(duongdan, L"../../../");
			wcscat(duongdan, mangsinhvien[index].MSSV);
			wcscat(duongdan, L".htm");
			xoamanghtml(manghtml);
			manghtml = readHTML(duongdan);
			if (manghtml == NULL) { manghtml = readHTML(htmlfilename);			xuly1sinhvien(manghtml, mangsinhvien[index], a, z); }
			int sodonghtml = getsizearr(manghtml);
			chisomoi = demchiso(manghtml);
			wprintf(L"bấm 1 để thêm email\n");
			wprintf(L"bấm 2 để thêm số điện thoại\n");
			wprintf(L"mời bạn nhập lựa chọn của mình (nhập sai mặc định không thêm)\n");
			int choice[2] = { 0,0 };
			wprintf(L"Lựa chọn thêm email:");
			wscanf(L"%d", &choice[0]);
			wprintf(L"Lựa chọn thêm số điện thoại: ");
			wscanf(L"%d", &choice[1]);
			fflush(stdin);
			if (choice[0] == 1) {
				wchar_t email[31] = L"";
				wprintf(L"email muốn thêm:");
				wscanf(L"%ls", &email);
				wchar_t* add = wcschr(manghtml[chisomoi.indexofcontact + 1], L':');
				if (add == NULL) {
					wprintf(L"định dạng văn bản không đúng với hàm tùy biến không thể sửa,đọc lại hướng dẫn");
					return;
				}
				add += 1;
				wcscpy(add, email);
				int temdum;
				add = wcschr(manghtml[chisomoi.indexofintro1 + 5], L':');
				add++;
				wchar_t* add2 = wcschr(add, L'<');
				replace(add, email, add2 - add);
			}
			if (choice[1] == 2) {
				wprintf(L"nhập số điện thoại muốn thêm: ");
				wchar_t sdt[20] = { 0 };
				wscanf(L"%ls", sdt);
				wchar_t* y = (wchar_t*)calloc(50 * sizeof(wchar_t), 1);
				wchar_t* yu = y;
				wcscpy(y, L"<li>Số điện thoại: ");
				wcscat(y, sdt);
				wcscat(y, L"</li>\n");
				pushback(&manghtml, y, sizeof(wchar_t*), pushbackkieuwchar_sao);
				if (manghtml == NULL) {
					wprintf(L"không đủ vùng nhớ cấp phát\n");
					return;
				}
				for (int i = getsizearr(manghtml) - 2; i >= chisomoi.indexofintro1 + 5; i--)
				{
					manghtml[i + 1] = manghtml[i];
				}

				//wchar_t* k1 = manghtml[z.indexofintro1 + 5];
				//wchar_t* k2 = manghtml[z.indexofintro1 + 5];
				manghtml[chisomoi.indexofintro1 + 6] = yu;
			}

			wchar_t link[255] = { L"../../../" };
			wcscat(link, mangsinhvien[index].MSSV);
			FILE *fp = _wfopen(wcscat(link, L".htm"), L"w, ccs=UTF-8");
			for (int j = 0; j < sodonghtml; j++)fputws(manghtml[j], fp);
			fclose(fp);
		}
		if (n == 5) {
			int soluongsinhvien = getsizearr(mangsinhvien);
			for (int i = 0; i < soluongsinhvien; i++) {
				wprintf(L"%d) %ls,%ls,%ls,%d,%ls,%ls,%ls,%ls\n", i, mangsinhvien[i].MSSV, mangsinhvien[i].Hovaten, mangsinhvien[i].Khoa, mangsinhvien[i].NienKhoa, mangsinhvien[i].Ngaysinh, mangsinhvien[i].HinhanhcaNhan, mangsinhvien[i].mota, mangsinhvien[i].sothich);
			}
			int index;
			wprintf(L"Mời bạn nhập chỉ số sinh viên(nhập sai thì mặc định là 0): ");
			wscanf(L"%d", &index);
			if (index >= soluongsinhvien || index < 0)index = 0;
			wchar_t duongdan[100] = { 0 };
			wcscpy(duongdan, L"../../../");
			wcscat(duongdan, mangsinhvien[index].MSSV);
			wcscat(duongdan, L".htm");
			xoamanghtml(manghtml);
			manghtml = readHTML(duongdan);
			if (manghtml == NULL) { manghtml = readHTML(htmlfilename);			xuly1sinhvien(manghtml, mangsinhvien[index], a, z); }
			int sodonghtml = getsizearr(manghtml);
			system("cls");
			while (1) {
				wprintf(L"thêm sở thích bấm 1,dừng bấm 0(bấm sai mặc định là 0)");
				int flag;
				wscanf(L"%d", &flag);
				if (flag > 1 || flag < 0)flag = 0;
				if (flag == 0)break;
				danhmuc tempdm = demchiso(manghtml);
				wchar_t dummychar;
				wscanf(L"%lc", &dummychar);//để bỏ dấu "\n"do k sử dụng đc fflush();
				wchar_t* sothich = (wchar_t*)calloc(1011 * sizeof(wchar_t), 1);
				if (sothich == NULL)return;
				wcscpy(sothich, L"<li>");
				wchar_t temchar[1001];
				wprintf(L"Nhập sở thích muốn thêm: ");
				fgetws(temchar, 1001, stdin);
				wcscat(sothich, temchar);
				wcscat(sothich, L"</li>");
				wchar_t * ptrofwch = sothich;
				pushback(&manghtml, sothich, sizeof(wchar_t*), pushbackkieuwchar_sao);
				if (manghtml == NULL)return;
				for (int i = getsizearr(manghtml) - 2; i >= tempdm.indexofintro2; i--) {
					manghtml[i + 1] = manghtml[i];
				}
				manghtml[tempdm.indexofintro2 + 1] = ptrofwch;
			}
			wchar_t link[255] = { L"../../../" };
			wcscat(link, mangsinhvien[index].MSSV);
			FILE *fp = _wfopen(wcscat(link, L".htm"), L"w, ccs=UTF-8");
			for (int j = 0; j < sodonghtml; j++)fputws(manghtml[j], fp);
			fclose(fp);
		}
		for (int l = 0; l < truongsinhvien; l++)a[l] = 1;//phucvucho n=3
		xoamanghtml(manghtml);
	}
}
void wmain() {
	_setmode(_fileno(stdout), _O_U16TEXT); //needed for output
	_setmode(_fileno(stdin), _O_U16TEXT); //needed for input
	sinhvien* mangsinhvien = readDatafromcsv(L"../../../sinhvien.csv");
	if (mangsinhvien == NULL)return;
	wchar_t *htmllink = (L"../../../template/1212123.htm");
	wchar_t **html = readHTML(htmllink);
	danhmuc z = demchiso(html);
	xoamanghtml(html);
	menu(htmllink, mangsinhvien, z);
	xoamangsinhvien(mangsinhvien);

}