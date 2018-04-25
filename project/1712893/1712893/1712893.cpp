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
	const wchar_t* fullname = L"<span class=\"Personal_FullName\">";
	int indexofname = 0;
	const wchar_t* Pdepartment = L"<div class=\"Personal_Department\">";
	int indexofPde = 0;
	const wchar_t *contact = L"<div class=\"Personal_Phone\">";
	int indexofcontact = 0;
	const wchar_t *picture = L"<div class=\"Personal_HinhcanhanKhung\">";
	int indexofpic = 0;
	const wchar_t *gioithieu = L"<ul class=\"TextInList\">";
	int indexofintro1 = 0;
	int indexofintro2 = 0;
	const wchar_t *mota = L"<div class=\"Description\">";
	int indexofDes = 0;
	const wchar_t *dev = L"MSSV - Tên sinh viên thực hiện";
	int indexofdev = 0;
}danhmuc;
typedef struct {
	wchar_t MSSV[11];
	wchar_t Hovaten[31];
	wchar_t Khoa[31];
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
	}
	fclose(fp);
	return mang;
}
void xoamanghtml(wchar_t** z) {
	if (z != NULL) {
		int n = getsizearr(z);
		for (int i = 0; i < n; i++) { if (*(wchar_t**)((char*)z + i * sizeof(wchar_t*)) != NULL)free(*(wchar_t**)((char*)z + i * sizeof(wchar_t*))); }
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
void xulytag(wchar_t** manghtml, int tagnum, const wchar_t* tagname, const wchar_t * tagendname, wchar_t* truongthaythe) {
	//tagnum là vị trí của tag cần đổi cho đến tag end
	wchar_t* vitri1 = wcsstr(manghtml[tagnum], tagname);
	if (vitri1 == NULL)return;
	int dem = 0;
	wchar_t* vitri2 = wcsstr(manghtml[tagnum], tagendname);
	while (vitri2 == NULL && dem + tagnum<getsizearr(manghtml)) {
		vitri2 = wcsstr(manghtml[tagnum + dem], tagendname);
		dem++;
	}
	if (vitri2 == NULL) {
		wprintf(L"không sửa được vì không đúng định dạng\n");
		system("pause");
		return;
	}
	if (dem > 0)dem--;
	vitri1 += wcslen(tagname);
	if (dem == 0) {
		replace(vitri1, truongthaythe, vitri2 - vitri1);//vi tri 2 cung dong vi tri 1
	}
	if (dem > 1) {//dua tagendname nam duoi tagname va thut vao dau dong
		wcscpy(vitri1, L"\n");
		wchar_t tempchar[1002];
		wcscpy(tempchar, vitri2);
		wcscpy(manghtml[tagnum + dem], tempchar);
		int tru = dem - 1;
		int size = getsizearr(manghtml);
		for (int i = tagnum + 1; i < size - tru; i++) {
			manghtml[i] = manghtml[i + tru];
		}
		manghtml = (wchar_t**)resizearr(manghtml, size - tru, sizeof(wchar_t*));
		if (manghtml == NULL) {
			wprintf(L"lỗi xử lý vùng nhớ"); return;
		}
	}
	if (dem >= 1) {
		wchar_t *add = (wchar_t*)calloc(1002 * sizeof(wchar_t), 1);
		wcscpy(add, truongthaythe);
		wchar_t *temp = add;
		pushback(&manghtml, add, sizeof(wchar_t*), pushbackkieuwchar_sao);
		if (manghtml == NULL) {
			wprintf(L"lỗi xử lý vùng nhớ"); return;
		}
		int size = getsizearr(manghtml);
		for (int i = size - 2; i >= tagnum; i--)//luc nay tag sẽ nằm trên tagname
		{
			manghtml[i + 1] = manghtml[i];
		}
		manghtml[tagnum + 1] = temp;
	}
}
void xuly1sinhvien_2(wchar_t** manghtml, sinhvien sv, int* a, danhmuc z) {
	if (a[0] == 0)wcscpy(sv.MSSV, L"");
	if (a[1] == 0)wcscpy(sv.Hovaten, L"");
	if (a[2] == 0)wcscpy(sv.Khoa, L"");
	//if (a[3] == 0)wcscpy(sv.email, L"");
	if (a[3] == 0)sv.NienKhoa = -1;
	if (a[4] == 0)wcscpy(sv.Ngaysinh, L"");
	if (a[5] == 0)wcscpy(sv.HinhanhcaNhan, L"");
	if (a[6] == 0)wcscpy(sv.mota, L"");
	if (a[7] == 0)wcscpy(sv.sothich, L"");
	wchar_t l[1002] = { 0 };
	wcscat(l, L"HCMUS - ");
	wcscat(l, sv.Hovaten);
	wchar_t* temp = wcsstr(manghtml[z.indexoftil], z.til);
	if (temp != NULL) xulytag(manghtml, z.indexoftil, z.til, L"</title>", l);
	wcscpy(l, sv.Hovaten);
	wcscat(l, L" - ");
	wcscat(l, sv.MSSV);
	z = demchiso(manghtml);
	temp = wcsstr(manghtml[z.indexofname], z.fullname);
	if (temp != NULL)xulytag(manghtml, z.indexofname, z.fullname, L"</span>", l);
	wcscpy(l, sv.Khoa);
	z = demchiso(manghtml);
	temp = wcsstr(manghtml[z.indexofPde], z.Pdepartment);
	if (temp != NULL)xulytag(manghtml, z.indexofPde, z.Pdepartment, L"</div>", l);
	z = demchiso(manghtml);
	wcscpy(l, L"Email: ");
	wcscat(l, L"\n");
	temp = wcsstr(manghtml[z.indexofcontact], z.contact);
	if (temp != NULL)xulytag(manghtml, z.indexofcontact, z.contact, L"</div>", l);
	z = demchiso(manghtml);
	wcscpy(l, L"<img src=\"");
	wcscat(l, sv.HinhanhcaNhan);
	wcscat(l, L"\"class=\"Personal_Hinhcanhan\" />");
	xulytag(manghtml, z.indexofpic, z.picture, L"</div>", l);
	z = demchiso(manghtml);
	wcscpy(l, L"<li>Họ và tên: ");
	wcscat(l, sv.Hovaten);
	wcscat(l, L"</li>\n<li>MSSV: ");
	wcscat(l, sv.MSSV);
	wcscat(l, L"</li>\n<li>Sinh viên khoa ");
	wcscat(l, sv.Khoa);
	wcscat(l, L"</li>\n<li>Ngày sinh: ");
	wcscat(l, sv.Ngaysinh);
	wcscat(l, L"</li>\n<li>Email: </li>\n");
	temp = wcsstr(manghtml[z.indexofintro1], z.gioithieu);
	if (temp != NULL)xulytag(manghtml, z.indexofintro1, z.gioithieu, L"</ul>", l);
	z = demchiso(manghtml);
	wcscpy(l, L"<li>");
	wcscat(l, sv.sothich);
	wcscat(l, L"</li>");
	temp = wcsstr(manghtml[z.indexofintro2], z.gioithieu);
	xulytag(manghtml, z.indexofintro2, z.gioithieu, L"</ul>", l);
	z = demchiso(manghtml);
	wcscpy(l, sv.mota);
	wcscat(l, L"\n");
	temp = wcsstr(manghtml[z.indexofDes], z.mota);
	xulytag(manghtml, z.indexofDes, z.mota, L"</div>", l);
	wcscpy(l, sv.MSSV);
	wcscat(l, L" - ");
	wcscat(l, sv.Hovaten);
	z = demchiso(manghtml);
	temp = wcsstr(manghtml[z.indexofdev], z.dev);//xac dinh xem vi tri co ton tai khong
	if (temp != NULL) {
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
	int flag4 = 0;
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
			for (int i = 0; i < soluongsinhvien; i++)
			{
				xuly1sinhvien_2(manghtml, mangsinhvien[i], a, z);
				wchar_t link[255] = { L"../../../" };
				int sodonghtml = getsizearr(manghtml);
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
			xuly1sinhvien_2(manghtml, mangsinhvien[index], a, z);
			int sodonghtml = getsizearr(manghtml);
			wchar_t link[255] = { L"../../../" };
			wcscat(link, mangsinhvien[index].MSSV);
			FILE *fp = _wfopen(wcscat(link, L".htm"), L"w, ccs=UTF-8");
			for (int j = 0; j < sodonghtml; j++)fputws(manghtml[j], fp);
			fclose(fp);
		}
		if (n == 3) {
			int soluongsinhvien = getsizearr(mangsinhvien);
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
			xuly1sinhvien_2(manghtml, temp, a, z);
			int sodonghtml = getsizearr(manghtml);
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
			if (manghtml == NULL || flag4 == 1) { manghtml = readHTML(htmlfilename);	flag4 = 0;		xuly1sinhvien_2(manghtml, mangsinhvien[index], a, z); }
			if (manghtml != NULL&&flag4 == 0)flag4 = 1;//da them email va so dien thoai
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
				danhmuc dmnew = demchiso(manghtml);
				xulytag(manghtml, dmnew.indexofcontact, dmnew.contact, L"</div>", email);
				//wcscpy(add, email);
				int temdum;
				int count = 0;
				wchar_t *add = wcsstr(manghtml[chisomoi.indexofintro1], L"Email:");
				while (add == NULL && chisomoi.indexofintro1 + count<getsizearr(manghtml)) {
					add = wcsstr(manghtml[chisomoi.indexofintro1 + count], L"Email:");
					count++;
				}
				if (count > 0)count--;
				if (chisomoi.indexofintro1 + count >= getsizearr(manghtml)||add==NULL) {
					wprintf(L"không thêm được vì không đúng định dạng");
					system("pause");
					continue;
				}
				add = wcschr(add, L':');
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
				int count = 0;
				wchar_t* add = wcsstr(manghtml[chisomoi.indexofintro1], L"Email:");
				while (add == NULL && chisomoi.indexofintro1 + count<getsizearr(manghtml)) {
					add = wcsstr(manghtml[chisomoi.indexofintro1 + count], L"Email:");
					count++;
				}
				if (count > 0)count--;
				if (chisomoi.indexofintro1 + count >= getsizearr(manghtml)) {
					wprintf(L"không thêm được vì không đúng định dạng");
					system("pause");
					continue;
				}
				for (int i = getsizearr(manghtml) - 2; i >= chisomoi.indexofintro1 + count; i--)
				{
					manghtml[i + 1] = manghtml[i];
				}

				//wchar_t* k1 = manghtml[z.indexofintro1 + 5];
				//wchar_t* k2 = manghtml[z.indexofintro1 + 5];
				manghtml[chisomoi.indexofintro1 + count + 1] = yu;
			}

			wchar_t link[255] = { L"../../../" };
			wcscat(link, mangsinhvien[index].MSSV);
			FILE *fp = _wfopen(wcscat(link, L".htm"), L"w, ccs=UTF-8");
			int sodonghtml = getsizearr(manghtml);
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
			if (manghtml == NULL) { manghtml = readHTML(htmlfilename);			xuly1sinhvien_2(manghtml, mangsinhvien[index], a, z); }
			system("cls");
			while (1) {
				wprintf(L"thêm sở thích bấm 1,dừng bấm 0(bấm sai mặc định là 0):");
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
			int sodonghtml = getsizearr(manghtml);
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