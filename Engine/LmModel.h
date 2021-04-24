//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmModel.h
//  @ Date : 2007-10-11
//  @ Author : 신동인
//
//


#if !defined(_LMMODEL_H)
#define _LMMODEL_H

class LmModelManager;

class LmModel {
public:
	LmModel(LPCWSTR _pcwsName); 
	LmModel(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile);
	virtual ~LmModel();

	// 복사와 대입을 막는다.
	LmModel(const LmModel& _rhs);
	LmModel& operator=(const LmModel& _rhs);
	
	virtual VOID Update(FLOAT _fTimeDelta) = 0;
	virtual VOID Render(LPDIRECT3DDEVICE9 _pDevice) = 0;
	
	LPCWSTR GetName(){ return m_wstrName.c_str(); }
	LPCWSTR GetXFileName(){ return m_wstrXFile.c_str(); }
	FLOAT GetPosX() { return m_fPosX; }
	FLOAT GetPosZ() { return m_fPosZ; }
	virtual BOOL Place(FLOAT _fPosX, FLOAT _fPosZ) = 0;	// 물체의 위치를 이동시킨다.
	virtual VOID Rotate() = 0;								// 물체를 90도씩 회전시킨다.

protected:	
	BOOL LoadData(LPDIRECT3DDEVICE9 _pDevice); // XFile로 부터 렌더링 데이터를 로드한다.
	VOID SetXFile(LPCWSTR _pcwsXFile);	// X파일 이름을 설정한다.

	wstring m_wstrXFile;				// 로드할 X파일 명
	wstring m_wstrName;					// 고유한 모델 이름
	D3DXMATRIX m_matWorld;				// 월드 매트릭스

	D3DXVECTOR3 m_vCenter;				// 경계구 중심	// 좌표 x. y
	FLOAT m_fRadius;					// 경계구 반지름

	BOOL m_bClone;						// 자기가 복사본인가?

	FLOAT m_fPosX, m_fPosZ;
	FLOAT m_fAngle;
};

#endif  //_LMMODEL_H
