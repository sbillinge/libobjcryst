/* 
* ObjCryst++ : a Crystallographic computing library in C++
*			http://objcryst.sourceforge.net
*			http://www.ccp14.ac.uk/ccp/web-mirrors/objcryst/
*
*  (c) 2000-2001 Vincent FAVRE-NICOLIN vincefn@users.sourceforge.net
*
*/
/*   Atom.h
*  header file for the Atom scatterer
*
*/

#include <cmath>
#include <typeinfo>

#include <stdio.h> //for sprintf()

//#include "ObjCryst/ScatteringPower.h"
//#include "ObjCryst/Scatterer.h"
//#include "ObjCryst/Atom.h"
#include "ObjCryst/ZScatterer.h"
#include "ObjCryst/ScatteringData.h"

#include "Quirks/VFNStreamFormat.h" //simple formatting of integers, doubles..

#include "Quirks/VFNDebug.h"
#ifdef OBJCRYST_GL
#include <GL/glut.h>
#endif

#include <fstream>
#include <iomanip>

#ifdef __WX__CRYST__
   #include "wxCryst/wxZScatterer.h"
   #undef GetClassName // Conflict from wxMSW headers ? (cygwin)
#endif

namespace ObjCryst
{
//######################################################################
//
//  ZAtom   
//
//
//######################################################################
ZAtom::ZAtom(ZScatterer &scatt,const ScatteringPower *pow,
             const long atomBond, const double bondLength,
             const long atomAngle, const double bondAngle,
             const long atomDihedral, const double dihedralAngle,
             const double popu, const string &name):
mpScattPow(pow),
mAtomBond(atomBond),mAtomAngle(atomAngle),mAtomDihed(atomDihedral),
mBondLength(bondLength),mAngle(bondAngle),mDihed(dihedralAngle),
mOccupancy(popu),mName(name),mpScatt(&scatt)
{
   VFN_DEBUG_MESSAGE("ZAtom::ZAtom():("<<mName<<")",5)
}
ZAtom::~ZAtom()
{
   VFN_DEBUG_MESSAGE("ZAtom::~ZAtom():("<<mName<<")",5)
}
const string& ZAtom::GetClassName()const
{
   static string className="ZAtom";
   return className;
}
const string& ZAtom::GetName()const {return mName;}

ZScatterer& ZAtom::GetZScatterer(){return *mpScatt;}
const ZScatterer& ZAtom::GetZScatterer()const{return *mpScatt;}

void ZAtom::SetName(const string& name) {mName=name;}
long ZAtom::GetZBondAtom()const {return mAtomBond;}
long ZAtom::GetZAngleAtom()const {return mAtomAngle;}
long ZAtom::GetZDihedralAngleAtom()const {return mAtomDihed;}
double ZAtom::GetZBondLength()const {return mBondLength;}
double ZAtom::GetZAngle()const {return mAngle;}
double ZAtom::GetZDihedralAngle()const {return mDihed;}
double ZAtom::GetOccupancy()const {return mOccupancy;}
const ScatteringPower* ZAtom::GetScatteringPower()const{return mpScattPow;}
//:TODO: fix the following so that their clocks are clicked accordingly
void ZAtom::SetZBondLength(const double bond) {mBondLength=bond;}
void ZAtom::SetZAngle(const double angle) {mAngle=angle;}
void ZAtom::SetZDihedralAngle(const double dihed) {mDihed=dihed;}
void ZAtom::SetOccupancy(const double pop) {mOccupancy=pop;}
void ZAtom::SetScatteringPower(const ScatteringPower* scatt) {mpScattPow=scatt;}
#ifdef __WX__CRYST__
WXCrystObjBasic* ZAtom::WXCreate(wxWindow *parent)
{
   VFN_DEBUG_ENTRY("ZAtom::WXCreate()",7)
   mpWXCrystObj=new WXZAtom (parent,this);
   VFN_DEBUG_EXIT("ZAtom::WXCreate()",7)
   return mpWXCrystObj;
}
WXCrystObjBasic* ZAtom::WXGet()
{
   return mpWXCrystObj;
}
void ZAtom::WXDelete()
{
   if(0!=mpWXCrystObj)
   {
      VFN_DEBUG_MESSAGE("ZAtom::WXDelete()",5)
      delete mpWXCrystObj;
   }
   mpWXCrystObj=0;
}
void ZAtom::WXNotifyDelete()
{
   VFN_DEBUG_MESSAGE("ZAtom::WXNotifyDelete():"<<mName,10)
   mpWXCrystObj=0;
}
#endif
//######################################################################
//
//  ZScatterer   
//
//
//######################################################################

ZScatterer::ZScatterer(const string &name,const Crystal &cryst, 
                       const double x,const double y,const double z,
                       const double phi,const double chi, const double psi):
mScattCompList(0),mNbAtom(0),mNbDummyAtom(0),
mPhi(0),mChi(0),mPsi(0),
mZAtomRegistry("List of ZAtoms"),
mCenterAtomIndex(0),
mPhiChiPsiMatrix(3,3),
mUseGlobalScattPow(false),mpGlobalScattPow(0)
{
   VFN_DEBUG_MESSAGE("ZScatterer::ZScatterer():("<<mName<<")",5)
   mName=name;
   mXYZ(0)=x;
   mXYZ(1)=y;
   mXYZ(2)=z;
   mPhi=phi;
   mChi=chi;
   mPsi=psi;
   this->SetCrystal(cryst);
   this->InitRefParList();
   VFN_DEBUG_MESSAGE("ZScatterer::ZScatterer():("<<mName<<")",5)
}

ZScatterer::ZScatterer(const ZScatterer &old):
Scatterer(old),m3DDisplayIndex(old.m3DDisplayIndex),
mScattCompList(old.mScattCompList),
mNbAtom(old.mNbAtom),mNbDummyAtom(old.mNbDummyAtom),
mPhi(old.mPhi),mChi(old.mChi),mPsi(old.mPsi),
mCenterAtomIndex(old.mCenterAtomIndex),
mPhiChiPsiMatrix(old.mPhiChiPsiMatrix),
mUseGlobalScattPow(false),mpGlobalScattPow(0)
{
   VFN_DEBUG_MESSAGE("ZScatterer::ZScatterer(&old):("<<mName<<")",5)
   
   throw ObjCrystException("ZScatterer::ZScatterer(&old): the copy of all atoms has \
   not been implemented yet...");
   
   mName=old.GetName();
   mXYZ(0)=old.GetX();
   mXYZ(1)=old.GetY();
   mXYZ(2)=old.GetZ();
   mPhi=old.GetPhi();
   mChi=old.GetChi();
   mPsi=old.GetPsi();
   this->SetCrystal(*(old.mpCryst));
   
   this->InitRefParList();
   
   this->SetUseGlobalScatteringPower(old.mUseGlobalScattPow);
}

ZScatterer::~ZScatterer()
{
   VFN_DEBUG_MESSAGE("ZScatterer::~ZScatterer():("<<mName<<")",5)
   if(0 != mpGlobalScattPow) delete mpGlobalScattPow;
}

ZScatterer* ZScatterer::CreateCopy() const
{
   VFN_DEBUG_MESSAGE("ZScatterer::CreateCopy()"<<mName<<")",5)
   return new ZScatterer(*this);
}
const string ZScatterer::GetClassName() const {return "ZScatterer";}

void ZScatterer::AddAtom(const string &name,const ScatteringPower *pow,
             const long atomBond, const double bondLength,
             const long atomAngle, const double bondAngle,
             const long atomDihedral, const double dihedralAngle,
             const double popu)
{
   VFN_DEBUG_MESSAGE("ZScatterer::AddAtom():"<<name<<")",5)
   ZAtom *zatom =new ZAtom(*this,pow,
                           atomBond,bondLength,
                           atomAngle,bondAngle,
                           atomDihedral,dihedralAngle,
                           popu,name);
   
   if(true==mUseGlobalScattPow)
   {
      throw ObjCrystException("ZScatterer::AddAtom(() Cannot add an atom ! \
You are using the Global ScatteringPower approximation !!");

   }
   bool usedBond=true,usedAngle=true,usedDihed=true;
   if(mZAtomRegistry.GetNb()<1) usedBond=false;
   if(mZAtomRegistry.GetNb()<2) usedAngle=false;
   if(mZAtomRegistry.GetNb()<3) usedDihed=false;
   {
      RefinablePar tmp(name+(string)"_bondLength",&(zatom->mBondLength),
                        1.,5.,
//                        bondLength*.9,bondLength*1.1,
                        gpRefParTypeScattConformBondLength,
                        REFPAR_DERIV_STEP_ABSOLUTE,true,false,usedBond,false,1.);
      tmp.AssignClock(mClockScatterer);
      this->AddPar(tmp);
   }
   {
      RefinablePar tmp(name+(string)"_BondAngle",&(zatom->mAngle),
                        0,2*M_PI,
//                        zatom->mAngle-.2,zatom->mAngle+.2,
                        gpRefParTypeScattConformBondAngle,
                        REFPAR_DERIV_STEP_ABSOLUTE,false,false,usedAngle,true,RAD2DEG,2*M_PI);
      tmp.AssignClock(mClockScatterer);
      this->AddPar(tmp);
   }
   {
      RefinablePar tmp(name+(string)"_DihedralAngle",&(zatom->mDihed),
                        0,2*M_PI,
//                        zatom->mDihed-.2,zatom->mDihed+.2,
                        gpRefParTypeScattConformDihedAngle,
                        REFPAR_DERIV_STEP_ABSOLUTE,false,false,usedDihed,true,RAD2DEG,2*M_PI);
      tmp.AssignClock(mClockScatterer);
      this->AddPar(tmp);
   }
   {//fixed by default
      RefinablePar tmp(name+(string)"_popu", 
                        &(zatom->mOccupancy),0,1,
                        gpRefParTypeScattOccup,
                        REFPAR_DERIV_STEP_ABSOLUTE,true,true,true,false,1.,1.);
      tmp.AssignClock(mClockScatterer);
      this->AddPar(tmp);
   }
   
   //:NOTE: we *must* add it in the registry after declaring the parameters,
   // since it triggers the creation of the WXZAtom, which requires the parameters...
   VFN_DEBUG_MESSAGE("ZScatterer::AddAtom():Registering...",2)
   mZAtomRegistry.Register(*zatom);

   // Add an entry for this atom in the list of all components. The actual values are
   // written in Update().No entry for a dummy atom
   VFN_DEBUG_MESSAGE("ZScatterer::AddAtom():Adding to the ScattCompList...",2)
      if(0!=pow) 
      {
         ++mScattCompList;
         mScattCompList(mNbAtom-mNbDummyAtom).mpScattPow=pow;
         mComponentIndex.resizeAndPreserve(mNbAtom-mNbDummyAtom+1);
         mComponentIndex(mNbAtom-mNbDummyAtom)=mNbAtom;
      }
      else mNbDummyAtom++;
  
   //Finish
   cout << "Added atom:"<<mNbAtom
        << " : "<<this->GetZBondAtom(mNbAtom)
        << " : "<<this->GetZAngleAtom(mNbAtom)
        << " : "<<this->GetZDihedralAngleAtom(mNbAtom)
        << " : "<<this->GetZBondLength(mNbAtom)
        << " : "<<this->GetZAngle(mNbAtom)
        << " : "<<this->GetZDihedralAngle(mNbAtom)<<endl;
      mNbAtom++;
      mClockScatterer.Click();
   VFN_DEBUG_MESSAGE("ZScatterer::AddAtom():End",3)
}

int ZScatterer::GetNbComponent() const
{
   if(true==mUseGlobalScattPow) return 1;
   return mNbAtom-mNbDummyAtom;
}
const ScatteringComponentList& ZScatterer::GetScatteringComponentList() const
{
   this->Update();
   
   return mScattCompList;
}

string ZScatterer::GetComponentName(const int i) const
{
   return mZAtomRegistry.GetObj(mComponentIndex(i)).GetName();
}

void ZScatterer::Print() const
{
   VFN_DEBUG_MESSAGE("ZScatterer::Print()",5)
   //:TODO: 
   //this->Update();
   //for(int i=0;i<this->mNbAtom;i++) ??;
}

double ZScatterer::GetPhi()    const {return mPhi;}
double ZScatterer::GetChi()    const {return mChi;}
double ZScatterer::GetPsi()    const {return mPsi;}

void ZScatterer::SetPhi(const double x) { mClockScatterer.Click();mPhi=x;}
void ZScatterer::SetChi(const double y) { mClockScatterer.Click();mChi=y;}
void ZScatterer::SetPsi(const double z) { mClockScatterer.Click();mPsi=z;}

long ZScatterer::GetZBondAtom(const int i)const 
{return mZAtomRegistry.GetObj(i).GetZBondAtom();}

long ZScatterer::GetZAngleAtom(const int i)const 
{return mZAtomRegistry.GetObj(i).GetZAngleAtom();}

long ZScatterer::GetZDihedralAngleAtom(const int i)const
{return mZAtomRegistry.GetObj(i).GetZDihedralAngleAtom();}

double ZScatterer::GetZBondLength(const int i)const 
{return mZAtomRegistry.GetObj(i).GetZBondLength();}
double ZScatterer::GetZAngle(const int i)const
{return mZAtomRegistry.GetObj(i).GetZAngle();}
double ZScatterer::GetZDihedralAngle(const int i)const
{return mZAtomRegistry.GetObj(i).GetZDihedralAngle();}

void ZScatterer::SetZBondLength(const int i,const double a)
{mClockScatterer.Click();mZAtomRegistry.GetObj(i).SetZBondLength(a);}

void ZScatterer::SetZAngle(const int i,const double a)
{mClockScatterer.Click();mZAtomRegistry.GetObj(i).SetZAngle(a);}

void ZScatterer::SetZDihedralAngle(const int i,const double a)
   {mClockScatterer.Click();mZAtomRegistry.GetObj(i).SetZDihedralAngle(a);}

const ObjRegistry<ZAtom>& ZScatterer::GetZAtomRegistry()const
{return mZAtomRegistry;}

ostream& ZScatterer::POVRayDescription(ostream &os,
                                         const bool onlyIndependentAtoms)const
{
   VFN_DEBUG_MESSAGE("ZScatterer::POVRayDescription()",5)
   //throw ObjCrystException("ZScatterer::POVRayDescription() not implemented! "+this->GetName());
   //:TODO:
   this->Update();

   //for(long i=0;i<mNbAtom;i++) mpAtom[i]->POVRayDescription(os,onlyIndependentAtoms);
   os << "// Description of ZScatterer :" << this->GetName() <<endl;
   
#if 0
   if(true==mUseGlobalScattPow) 
   {
      mpAtom[mCenterAtomIndex]->POVRayDescription(os,onlyIndependentAtoms);
      return os;
   }
   //Declare colours
   for(int i=0;i<mNbAtom;i++)
   {
      if(mpAtom[i]->IsDummy()) continue;
      os <<"   #declare colour_"<<mpAtom[i]->GetName()<<"="<<mpAtom[i]->Colour()<<";"<<endl;
   }
   
   if(true==onlyIndependentAtoms)
   {
      CrystVector_double x(mNbAtom),y(mNbAtom),z(mNbAtom);
      for(int i=0;i<mNbAtom;i++)
      {
         x(i)=mpAtom[i]->GetX();
         y(i)=mpAtom[i]->GetY();
         z(i)=mpAtom[i]->GetZ();
         this->GetCrystal().FractionalToOrthonormalCoords(x(i),y(i),z(i));
      }
      for(int i=0;i<mNbAtom;i++)
      {
         if(mpAtom[i]->IsDummy())
         {
            os << "   // Skipping Dummy Atom :" << mpAtom[i]->GetName() <<endl<<endl;
            continue;
         }
         os << "   // Atom :" << mpAtom[i]->GetName() <<endl;
         os << "      sphere " << endl;
         os << "      { <"<<x(i)<<","<<y(i)<<","<<z(i)<< ">,"
            << mpAtom[i]->GetRadius()/3<<endl;
         os << "          finish { ambient 0.2 diffuse 0.8 phong 1}" <<endl;
         os << "          pigment { colour colour_"<< mpAtom[i]->GetName() <<" }" << endl;
         os << "      }" <<endl;
         //Draw the bond for this Atom,if it's not linked to a dummy
         int bond=ZBondAtom(i);
         if((mpAtom[bond]->IsDummy()) || (i==0)) continue;
         os << "      cylinder"<<endl;
         os << "      { <"<<x(i)<<","<<y(i)<<","<<z(i)<< ">,"<<endl;
         os << "      <"<<x(bond)<<","<<y(bond)<<","<<z(bond)<< ">,"<<endl;
         os << "      0.1"<<endl;
         os << "      pigment { colour Gray }"<<endl;
         os << "      }"<<endl;
      }
   }
   else
   {
      CrystMatrix_double* xyzCoords=new CrystMatrix_double[mNbAtom];
      for(int i=0;i<mNbAtom;i++)
         *(xyzCoords+i)=this->GetCrystal().GetSpaceGroup().GetAllSymmetrics(mpAtom[i]->GetX(),
                                                            mpAtom[i]->GetY(),
                                                            mpAtom[i]->GetZ(),
                                                            false,false,false);
      const int nbSymmetrics=(xyzCoords+0)->rows();
      int symNum=0;
      CrystMatrix_int translate(27,3);
      translate=  -1,-1,-1,
                  -1,-1, 0,
                  -1,-1, 1,
                  -1, 0,-1,
                  -1, 0, 0,
                  -1, 0, 1,
                  -1, 1,-1,
                  -1, 1, 0,
                  -1, 1, 1,
                   0,-1,-1,
                   0,-1, 0,
                   0,-1, 1,
                   0, 0,-1,
                   0, 0, 0,
                   0, 0, 1,
                   0, 1,-1,
                   0, 1, 0,
                   0, 1, 1,
                   1,-1,-1,
                   1,-1, 0,
                   1,-1, 1,
                   1, 0,-1,
                   1, 0, 0,
                   1, 0, 1,
                   1, 1,-1,
                   1, 1, 0,
                   1, 1, 1;
      double dx,dy,dz;
      CrystVector_double x(mNbAtom),y(mNbAtom),z(mNbAtom);
      CrystVector_double xSave,ySave,zSave;
      for(int i=0;i<nbSymmetrics;i++)
      {
         for(int j=0;j<mNbAtom;j++)
         {
            x(j)=(*(xyzCoords+j))(i,0);
            y(j)=(*(xyzCoords+j))(i,1);
            z(j)=(*(xyzCoords+j))(i,2);
         }
         //Bring back central atom in unit cell; move peripheral atoms with the same amount
            dx=x(0);
            dy=y(0);
            dz=z(0);
            x(0) = fmod((double) x(0),(int)1); if(x(0)<0) x(0)+=1.;
            y(0) = fmod((double) y(0),(int)1); if(y(0)<0) y(0)+=1.;
            z(0) = fmod((double) z(0),(int)1); if(z(0)<0) z(0)+=1.;
            dx = x(0)-dx;
            dy = y(0)-dy;
            dz = z(0)-dz;
            for(int j=1;j<mNbAtom;j++)
            {
               x(j) += dx;
               y(j) += dy;
               z(j) += dz;
            }
         //Generate also translated atoms near the unit cell
         const double limit =0.1;
         for(int j=0;j<translate.rows();j++)
         {
            xSave=x;
            ySave=y;
            zSave=z;
            x += translate(j,0);
            y += translate(j,1);
            z += translate(j,2);
            if(   (x(0)>(-limit)) && (x(0)<(1+limit))
                &&(y(0)>(-limit)) && (y(0)<(1+limit))
                &&(z(0)>(-limit)) && (z(0)<(1+limit)))
            {
               for(int k=0;k<mNbAtom;k++)
                  this->GetCrystal().FractionalToOrthonormalCoords(x(k),y(k),z(k));
               os << "  // Symmetric&Translated #" << symNum++ <<endl;
               //:NOTE: The code below is the same as without symmetrics
               for(int i=0;i<mNbAtom;i++)
               {
                  if(mpAtom[i]->IsDummy())
                  {
                     os << "   // Skipping Dummy Atom :" << mpAtom[i]->GetName() <<endl<<endl;
                     continue;
                  }
                  os << "   // Atom :" << mpAtom[i]->GetName() <<endl;
                  os << "      sphere " << endl;
                  os << "      { <"<<x(i)<<","<<y(i)<<","<<z(i)<< ">,"
                     << mpAtom[i]->GetRadius()/3<<endl;
                  os << "          finish { ambient 0.2 diffuse 0.8 phong 1}" <<endl;
                  os << "          pigment { colour colour_"<< mpAtom[i]->GetName() <<" }" << endl;
                  os << "      }" <<endl;
                  //Draw the bond for this Atom,if it's not linked to a dummy
                  int bond=ZBondAtom(i);
                  if((mpAtom[bond]->IsDummy()) || (i==0)) continue;
                  os << "      cylinder"<<endl;
                  os << "      { <"<<x(i)<<","<<y(i)<<","<<z(i)<< ">,"<<endl;
                  os << "      <"<<x(bond)<<","<<y(bond)<<","<<z(bond)<< ">,"<<endl;
                  os << "      0.1"<<endl;
                  os << "      pigment { colour Gray }"<<endl;
                  os << "      }"<<endl;
               }
            }//if in limits
            x=xSave;
            y=ySave;
            z=zSave;
         }//for translation
      }//for symmetrics
      delete[] xyzCoords;
   }//else
#endif
   return os;
}

void ZScatterer::GLInitDisplayList(const bool onlyIndependentAtoms,
                                   const double xMin,const double xMax,
                                   const double yMin,const double yMax,
                                   const double zMin,const double zMax)const
{
   #ifdef OBJCRYST_GL
   VFN_DEBUG_ENTRY("ZScatterer::GLInitDisplayList()",4)
   this->Update();
   if(true==mUseGlobalScattPow) 
   {
      //mpAtom[mCenterAtomIndex]->GLInitDisplayList(onlyIndependentAtoms);
      return;
   }
   
   GLfloat colour_bond[]= { 0.5, .5, .5, 1.0 };
   GLfloat colour_side[]= { 0.0, .0, .0, 1.0 };
   
   GLUquadricObj* pQuadric = gluNewQuadric();
   
   if(true==onlyIndependentAtoms)
   {
      //cout << m3DDisplayIndex <<endl;
      CrystVector_double x,y,z;
      x=mXCoord;
      y=mYCoord;
      z=mZCoord;
      if(m3DDisplayIndex.numElements()>0)
      {
         for(long k=0;k<m3DDisplayIndex.rows();k++)
         {
            switch(m3DDisplayIndex(k,0))
            {
               case 0:break;
               case 1: //Draw a sphere
               {
                  const int n1=m3DDisplayIndex(k,1);
                  if(0==mZAtomRegistry.GetObj(n1).GetScatteringPower())break;
                  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                                mZAtomRegistry.GetObj(n1).GetScatteringPower()->GetColourRGB());
                  glPushMatrix();
                     glTranslatef(x(n1), y(n1), z(n1));
                     //glutSolidSphere
                     gluSphere(pQuadric,mZAtomRegistry.GetObj(n1).GetScatteringPower()
                                       ->GetRadius()/3.,10,10);
                  glPopMatrix();
               }
               case 2: // Draw a bond
               {
                  long n1,n2;
                  n1=m3DDisplayIndex(k,1);
                  n2=m3DDisplayIndex(k,2);
                  glPushMatrix();
                     glTranslatef(x(n1), y(n1), z(n1));
                     glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,colour_bond);
                     GLUquadricObj *quadobj = gluNewQuadric();
                     glColor3f(1.0f,1.0f,1.0f);
                     const double height= sqrt( (x(n2)-x(n1))*(x(n2)-x(n1))
                                              +(y(n2)-y(n1))*(y(n2)-y(n1))
                                              +(z(n2)-z(n1))*(z(n2)-z(n1)));
                     glRotatef(180,x(n2)-x(n1),y(n2)-y(n1),z(n2)-z(n1)+height);// ?!?!?!
                     gluCylinder(quadobj,.1,.1,height,10,1 );
                     gluDeleteQuadric(quadobj);
                  glPopMatrix();

               }
               case 3: // Draw a triangular face
               {
                  long n1,n2,n3;
                  double x1,y1,z1,x2,y2,z2,xn,yn,zn,xc,yc,zc;
                  n1=m3DDisplayIndex(k,1);
                  n2=m3DDisplayIndex(k,2);
                  n3=m3DDisplayIndex(k,3);
                  x1=x(n1)-x(n2);
                  y1=y(n1)-y(n2);
                  z1=z(n1)-z(n2);

                  x2=x(n1)-x(n3);
                  y2=y(n1)-y(n3);
                  z2=z(n1)-z(n3);

                  xn=y1*z2-z1*y2;
                  yn=z1*x2-x1*z2;
                  zn=x1*y2-y1*x2;

                  xc=(x(n1)+x(n2)+x(n3))/3.-x(mCenterAtomIndex);
                  yc=(y(n1)+y(n2)+y(n3))/3.-y(mCenterAtomIndex);
                  zc=(z(n1)+z(n2)+z(n3))/3.-z(mCenterAtomIndex);

                  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
                                mZAtomRegistry.GetObj(0).GetScatteringPower()->GetColourRGB());
                  //glColor3f(1.0f,1.0f,1.0f);	   // White
                  glBegin(GL_TRIANGLES);				// Bottom
                     if((xn*xc+yn*yc+zn*zc)>0) glNormal3f(xn, yn, zn);
                     else glNormal3f(-xn, -yn, -zn);
                     glVertex3f(x(n1),y(n1),z(n1));
                     glVertex3f(x(n2),y(n2),z(n2));
                     glVertex3f(x(n3),y(n3),z(n3));
                  glEnd();
                  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,colour_side);
                  glBegin(GL_LINE_LOOP);
                     glVertex3f(x(n1),y(n1),z(n1));
                     glVertex3f(x(n2),y(n2),z(n2));
                     glVertex3f(x(n3),y(n3),z(n3));
                  glEnd();
               }
               case 4: // Draw a quadric face
               {
                  long n1,n2,n3,n4;
                  double x1,y1,z1,x2,y2,z2,xn,yn,zn,xc,yc,zc;
                  n1=m3DDisplayIndex(k,1);
                  n2=m3DDisplayIndex(k,2);
                  n3=m3DDisplayIndex(k,3);
                  n4=m3DDisplayIndex(k,3);
                  x1=x(n1)-x(n2);
                  y1=y(n1)-y(n2);
                  z1=z(n1)-z(n2);

                  x2=x(n1)-x(n3);
                  y2=y(n1)-y(n3);
                  z2=z(n1)-z(n3);

                  xn=y1*z2-z1*y2;
                  yn=z1*x2-x1*z2;
                  zn=x1*y2-y1*x2;

                  xc=(x(n1)+x(n2)+x(n3)+x(n4))/4.-x(mCenterAtomIndex);
                  yc=(y(n1)+y(n2)+y(n3)+y(n4))/4.-y(mCenterAtomIndex);
                  zc=(z(n1)+z(n2)+z(n3)+z(n4))/4.-z(mCenterAtomIndex);

                  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
                                mZAtomRegistry.GetObj(0).GetScatteringPower()->GetColourRGB());
                  //glColor3f(1.0f,1.0f,1.0f);	   // White
                  glBegin(GL_TRIANGLES);				// Bottom
                     if((xn*xc+yn*yc+zn*zc)>0) glNormal3f(xn, yn, zn);
                     else glNormal3f(-xn, -yn, -zn);
                     glVertex3f(x(n1),y(n1),z(n1));
                     glVertex3f(x(n2),y(n2),z(n2));
                     glVertex3f(x(n3),y(n3),z(n3));
                     glVertex3f(x(n4),y(n4),z(n4));
                  glEnd();
                  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,colour_side);
                  glBegin(GL_LINE_LOOP);
                     glVertex3f(x(n1),y(n1),z(n1));
                     glVertex3f(x(n2),y(n2),z(n2));
                     glVertex3f(x(n3),y(n3),z(n3));
                     glVertex3f(x(n4),y(n4),z(n4));
                  glEnd();
               }
            }
         }
      }
      else
      {
         for(int k=0;k<mNbAtom;k++)
         {
            if(0==mZAtomRegistry.GetObj(k).GetScatteringPower())continue;
            glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                          mZAtomRegistry.GetObj(k).GetScatteringPower()->GetColourRGB());
            glPushMatrix();
               glTranslatef(x(k), y(k), z(k));
               gluSphere(pQuadric,
                  mZAtomRegistry.GetObj(k).GetScatteringPower()->GetRadius()/3.,10,10);
               //Draw the bond for this Atom,if it's not linked to a dummy
               int bond=this->GetZBondAtom(k);
               if((0!=mZAtomRegistry.GetObj(bond).GetScatteringPower()) && (k>0))
               {
                  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,colour_bond);
                  GLUquadricObj *quadobj = gluNewQuadric();
                  glColor3f(1.0f,1.0f,1.0f);
                  const double height= sqrt( (x(bond)-x(k))*(x(bond)-x(k))
                                           +(y(bond)-y(k))*(y(bond)-y(k))
                                           +(z(bond)-z(k))*(z(bond)-z(k)));
                  glRotatef(180,x(bond)-x(k),y(bond)-y(k),z(bond)-z(k)+height);// !!!
                  gluCylinder(quadobj,.1,.1,height,10,1 );
                  gluDeleteQuadric(quadobj);
               }
            glPopMatrix();
         }
      }//Use triangle faces ?
   }//Only independent atoms ?
   else
   {
      VFN_DEBUG_ENTRY("ZScatterer::GLInitDisplayList():Show all symmetrics",3)
      CrystMatrix_double xyzCoords[100]; //:TODO:
      {
         double x0,y0,z0;
         for(int i=0;i<mNbAtom;i++)
         {//We also generate the positions of dummy atoms.. This may be needed...
            x0=mXCoord(i);
            y0=mYCoord(i);
            z0=mZCoord(i);
            this->GetCrystal().OrthonormalToFractionalCoords(x0,y0,z0);
            xyzCoords[i]=this->GetCrystal().GetSpaceGroup().
									GetAllSymmetrics(x0,y0,z0,false,false,false);
         }
      }
      CrystMatrix_int translate(27,3);
      translate=  -1,-1,-1,
                  -1,-1, 0,
                  -1,-1, 1,
                  -1, 0,-1,
                  -1, 0, 0,
                  -1, 0, 1,
                  -1, 1,-1,
                  -1, 1, 0,
                  -1, 1, 1,
                   0,-1,-1,
                   0,-1, 0,
                   0,-1, 1,
                   0, 0,-1,
                   0, 0, 0,
                   0, 0, 1,
                   0, 1,-1,
                   0, 1, 0,
                   0, 1, 1,
                   1,-1,-1,
                   1,-1, 0,
                   1,-1, 1,
                   1, 0,-1,
                   1, 0, 0,
                   1, 0, 1,
                   1, 1,-1,
                   1, 1, 0,
                   1, 1, 1;
      double dx,dy,dz;
      CrystVector_double x(mNbAtom),y(mNbAtom),z(mNbAtom);
      CrystVector_double xSave,ySave,zSave;
      const int nbSymmetrics=xyzCoords[0].rows();
      for(int i=0;i<nbSymmetrics;i++)
      {
         VFN_DEBUG_ENTRY("ZScatterer::GLInitDisplayList():Symmetric#"<<i,3)
         for(int j=0;j<mNbAtom;j++)
         {
            x(j)=xyzCoords[j](i,0);
            y(j)=xyzCoords[j](i,1);
            z(j)=xyzCoords[j](i,2);
         }
         //Bring back central atom in unit cell; move peripheral atoms with the same amount
            dx=x(0);
            dy=y(0);
            dz=z(0);
            x(0) = fmod((double) x(0),(int)1); if(x(0)<0) x(0)+=1.;
            y(0) = fmod((double) y(0),(int)1); if(y(0)<0) y(0)+=1.;
            z(0) = fmod((double) z(0),(int)1); if(z(0)<0) z(0)+=1.;
            dx = x(0)-dx;
            dy = y(0)-dy;
            dz = z(0)-dz;
            for(int j=1;j<mNbAtom;j++)
            {
               x(j) += dx;
               y(j) += dy;
               z(j) += dz;
            }
         //Generate also translated atoms near the unit cell
         xSave=x;
         ySave=y;
         zSave=z;
         for(int j=0;j<translate.rows();j++)
         {
            x += translate(j,0);
            y += translate(j,1);
            z += translate(j,2);
            if(   (x(0)>xMin) && (x(0)<xMax)
                &&(y(0)>yMin) && (y(0)<yMax)
                &&(z(0)>zMin) && (z(0)<zMax))
            {
               for(int k=0;k<mNbAtom;k++)
                  this->GetCrystal().FractionalToOrthonormalCoords(x(k),y(k),z(k));
               //:NOTE: The code below is the same as without symmetrics
               if(m3DDisplayIndex.numElements()>0)
               {
                  long n1,n2,n3;
                  double x1,y1,z1,x2,y2,z2,xn,yn,zn,xc,yc,zc;
                  for(long k=0;k<m3DDisplayIndex.rows();k++)
                  {
                     n1=m3DDisplayIndex(k,0);
                     n2=m3DDisplayIndex(k,1);
                     n3=m3DDisplayIndex(k,2);
                     x1=x(n1)-x(n2);
                     y1=y(n1)-y(n2);
                     z1=z(n1)-z(n2);

                     x2=x(n1)-x(n3);
                     y2=y(n1)-y(n3);
                     z2=z(n1)-z(n3);

                     xn=y1*z2-z1*y2;
                     yn=z1*x2-x1*z2;
                     zn=x1*y2-y1*x2;

                     xc=(x(n1)+x(n2)+x(n3))/3.-x(mCenterAtomIndex);
                     yc=(y(n1)+y(n2)+y(n3))/3.-y(mCenterAtomIndex);
                     zc=(z(n1)+z(n2)+z(n3))/3.-z(mCenterAtomIndex);

                     glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,
                                 mZAtomRegistry.GetObj(0).GetScatteringPower()->GetColourRGB());
                     glBegin(GL_TRIANGLES);
                        if((xn*xc+yn*yc+zn*zc)>0) glNormal3f( xn,  yn,  zn);
                        else                      glNormal3f(-xn, -yn, -zn);
                        
                        glVertex3f(x(n1),y(n1),z(n1));
                        glVertex3f(x(n2),y(n2),z(n2));
                        glVertex3f(x(n3),y(n3),z(n3));
                     glEnd();
                     glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,colour_side);
                     glBegin(GL_LINE_LOOP);
                        glVertex3f(x(n1),y(n1),z(n1));
                        glVertex3f(x(n2),y(n2),z(n2));
                        glVertex3f(x(n3),y(n3),z(n3));
                     glEnd();
                  }
               }
               else
               {
                  for(int k=0;k<mNbAtom;k++)
                  {
                     if(0==mZAtomRegistry.GetObj(k).GetScatteringPower())continue;
                     glMaterialfv (GL_FRONT,GL_AMBIENT_AND_DIFFUSE,
                        mZAtomRegistry.GetObj(k).GetScatteringPower()->GetColourRGB());
                     glPushMatrix();
                        glTranslatef(x(k), y(k), z(k));
                        gluSphere(pQuadric,
                           mZAtomRegistry.GetObj(k).GetScatteringPower()->GetRadius()/3.,10,10);
                        //Draw the bond for this Atom,if it's not linked to a dummy
                        int bond=this->GetZBondAtom(k);
                        if((0!=mZAtomRegistry.GetObj(bond).GetScatteringPower()) && (k>0))
                        {
                           glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,colour_bond);
                           GLUquadricObj *quadobj = gluNewQuadric();
                           glColor3f(1.0f,1.0f,1.0f);
                           const double height= sqrt( (x(bond)-x(k))*(x(bond)-x(k))
                                                    +(y(bond)-y(k))*(y(bond)-y(k))
                                                    +(z(bond)-z(k))*(z(bond)-z(k)));
                           glRotatef(180,x(bond)-x(k),y(bond)-y(k),z(bond)-z(k)+height);// !!!
                           gluCylinder(quadobj,.1,.1,height,10,1 );
                           gluDeleteQuadric(quadobj);
                        }
                     glPopMatrix();
                  }
               }//Use triangle faces ?

               /*
               for(int k=0;k<1;k++)//mNbAtom
               {
                  if(0==mZAtomRegistry.GetObj(k).GetScatteringPower())continue;
                  glColor3f(0.8f,0.4f,0.4f);	   // Red
                  glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                                mZAtomRegistry.GetObj(k).GetScatteringPower()->GetColourRGB());
                  glPushMatrix();
                     glTranslatef(x(k), y(k), z(k));
                     glutSolidSphere(
                        mZAtomRegistry.GetObj(k).GetScatteringPower()->GetRadius()/3.,10,10);
                     //Draw the bond for this Atom,if it's not linked to a dummy
                     const int bond=ZBondAtom(k);
                     if((0!=mZAtomRegistry.GetObj(bond).GetScatteringPower()->) && (k>0))
                     {
                        glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE,colour_bond);
                        GLUquadricObj *quadobj = gluNewQuadric();
                        glColor3f(1.0f,1.0f,1.0f);	   // Red
                        const double height= sqrt( (x(bond)-x(k))*(x(bond)-x(k))
                                                 +(y(bond)-y(k))*(y(bond)-y(k))
                                                 +(z(bond)-z(k))*(z(bond)-z(k)));
                        glRotatef(180,x(bond)-x(k),y(bond)-y(k),z(bond)-z(k)+height);// !!!
                        gluCylinder(quadobj,.1,.1,height,10,1 );
                        gluDeleteQuadric(quadobj);
                     }
                     glPopMatrix();
               }
               */
            }//if in limits
            x=xSave;
            y=ySave;
            z=zSave;
         }//for translation
      }//for symmetrics
      VFN_DEBUG_EXIT("ZScatterer::GLInitDisplayList():Show all symmetrics",3)
   }//else
   gluDeleteQuadric(pQuadric);
   VFN_DEBUG_EXIT("ZScatterer::GLInitDisplayList()",4)
   #endif //GLCryst
}

void ZScatterer::SetUseGlobalScatteringPower(const bool useIt)
{
   VFN_DEBUG_MESSAGE("ZScatterer::SetUseGlobalScatteringPower(bool):"<<this->GetName()<<":"<<useIt,5)
   if(true==useIt)
   {
      if(false==mUseGlobalScattPow)
      {
         mpGlobalScattPow=new GlobalScatteringPower(*this);
         mUseGlobalScattPow=true;
         mScattCompList.Reset();
         ++mScattCompList;
         this->InitRefParList();
         mClockScatterer.Click();
      }
      //Just change the functions which return the component list
   }
   else
   {
      if(true==mUseGlobalScattPow)
      {
         delete mpGlobalScattPow;
         mpGlobalScattPow=0;
         mUseGlobalScattPow=false;
         mScattCompList.Reset();
         for(long i=0;i<(mNbAtom-mNbDummyAtom);i++) ++mScattCompList;
         this->InitRefParList();
         mClockScatterer.Click();
      }
   }
}

void ZScatterer::Update() const
{
   if(  (mClockScattCompList>mClockScatterer)
      &&(mClockScattCompList>mpCryst->GetClockLatticePar())) return;

   VFN_DEBUG_ENTRY("ZScatterer::Update(bool)"<<this->GetName(),3)
   TAU_PROFILE("ZScatterer::Update()","void ()",TAU_DEFAULT);
   //if(0==mNbAtom) throw ObjCrystException("ZScatterer::Update() No Atoms in Scatterer !");
   if(0==mNbAtom) return;
   
   if(true==mUseGlobalScattPow)
   {
      mScattCompList(0).mX=mXYZ(0);
      mScattCompList(0).mY=mXYZ(1);
      mScattCompList(0).mZ=mXYZ(2);
      mScattCompList(0).mOccupancy=mOccupancy;
      mScattCompList(0).mpScattPow=mpGlobalScattPow;
      
      // Update central atom for Display.
      //mXCoord(mCenterAtomIndex)=this->GetX();
      //mYCoord(mCenterAtomIndex)=this->GetY();
      //mZCoord(mCenterAtomIndex)=this->GetZ();
      //mpCryst->FractionalToOrthonormalCoords(mXCoord(mCenterAtomIndex),
      //                                       mXCoord(mCenterAtomIndex),
      //                                       mXCoord(mCenterAtomIndex));
      
      mClockScattCompList.Click();
      VFN_DEBUG_MESSAGE("ZScatterer::Update()->Global Scatterer:End",3)
      return;
   }

   {
      CrystMatrix_double phiMatrix(3,3),chiMatrix(3,3),psiMatrix(3,3);
      phiMatrix= cos(mPhi)   , -sin(mPhi)   , 0,
                 sin(mPhi)   , cos(mPhi)    , 0,
                 0           ,0             ,1;

      chiMatrix= cos(mChi)   ,0             ,-sin(mChi),
                 0           ,1             ,0,
                 sin(mChi)   ,0             ,cos(mChi);

      psiMatrix= 1           , 0            , 0,
                 0           ,cos(mPsi)     ,-sin(mPsi),
                 0           ,sin(mPsi)     ,cos(mPsi);
                 
      mPhiChiPsiMatrix=product(chiMatrix,product(phiMatrix,psiMatrix));
      //cout << phiMatrix <<endl<< chiMatrix <<endl<< psiMatrix <<endl<<mPhiChiPsiMatrix<<endl;
   }

   mXCoord.resize(mNbAtom);
   mYCoord.resize(mNbAtom);
   mZCoord.resize(mNbAtom);
   
   // Atom 0
   mXCoord(0)=0.;
   mYCoord(0)=0.;
   mZCoord(0)=0.;
   VFN_DEBUG_MESSAGE("->Atom #0:"<<mXCoord(0)<<" : "<<mYCoord(0)<<" : "<<mZCoord(0),1)
   
   if(mNbAtom>1)
   {// Atom 1
      mXCoord(1)=GetZBondLength(1);
      mYCoord(1)=0.;
      mZCoord(1)=0.;
   }
   VFN_DEBUG_MESSAGE("->Atom #1:"<<mXCoord(1)<<" : "<<mYCoord(1)<<" : "<<mZCoord(1),1)
   if(mNbAtom>2)
   {// Atom 2
      if(0==GetZBondAtom(2)) //Linked with Atom 1
         mXCoord(2)=GetZBondLength(2)*cos(GetZAngle(2));
      else //Linked with Atom 1
         mXCoord(2)=mXCoord(1)-GetZBondLength(2)*cos(GetZAngle(2));
      mYCoord(2)=GetZBondLength(2)*sin(GetZAngle(2));
      mZCoord(2)=0.;
   }
   VFN_DEBUG_MESSAGE("->Atom #2:"<<mXCoord(2)<<" : "<<mYCoord(2)<<" : "<<mZCoord(2),1)
   for(int i=1;i<3;i++)// Global rotation of scatterer
   {
      const double x=mXCoord(i);
      const double y=mYCoord(i);
      const double z=mZCoord(i);
      mXCoord(i)=mPhiChiPsiMatrix(0,0)*x+mPhiChiPsiMatrix(0,1)*y+mPhiChiPsiMatrix(0,2)*z;
      mYCoord(i)=mPhiChiPsiMatrix(1,0)*x+mPhiChiPsiMatrix(1,1)*y+mPhiChiPsiMatrix(1,2)*z;
      mZCoord(i)=mPhiChiPsiMatrix(2,0)*x+mPhiChiPsiMatrix(2,1)*y+mPhiChiPsiMatrix(2,2)*z;
      if(i==mNbAtom-1) break;
   }
   if(mNbAtom>3)
   {
      double xa,ya,za,xb,yb,zb,xd,yd,zd,cosph,sinph,costh,sinth,coskh,sinkh,cosa,sina;
      double xpd,ypd,zpd,xqd,yqd,zqd;
      double rbc,xyb,yza,tmp,xpa,ypa,zqa;
      int na,nb,nc;
      bool flag;
      double dist,angle,dihed;
      for(int i=3;i<mNbAtom;i++)
      {
         na=GetZBondAtom(i);
         nb=GetZAngleAtom(i);
         nc=GetZDihedralAngleAtom(i);
         dist=GetZBondLength(i);
         angle=GetZAngle(i);
         dihed=GetZDihedralAngle(i);
         
         xb = mXCoord(nb) - mXCoord(na);
         yb = mYCoord(nb) - mYCoord(na);
         zb = mZCoord(nb) - mZCoord(na);

         rbc= 1./sqrt(xb*xb + yb*yb + zb*zb);

         cosa = cos(angle);
         sina = sin(angle);

         if( fabs(cosa) >= 0.999999 )
         {   // Colinear
            mXCoord(i)=mXCoord(na)+cosa*dist*rbc*xb;
            mYCoord(i)=mYCoord(na)+cosa*dist*rbc*yb;
            mZCoord(i)=mZCoord(na)+cosa*dist*rbc*zb;
            VFN_DEBUG_MESSAGE("->Atom #"<<i<<":"<<mXCoord(i)<<" : "<<mYCoord(i)<<" : " <<mZCoord(i)<<"(colinear)",1)
         }
         else
         {
            xa = mXCoord(nc) - mXCoord(na);
            ya = mYCoord(nc) - mYCoord(na);
            za = mZCoord(nc) - mZCoord(na);
            xd = dist*cosa;
            yd = dist*sina*cos(dihed);
            zd = -dist*sina*sin(dihed);

            xyb = sqrt(xb*xb + yb*yb);
            if( xyb < 0.1 )
            {   // Rotate about y-axis
                tmp = za; za = -xa; xa = tmp;
                tmp = zb; zb = -xb; xb = tmp;
                xyb = sqrt(xb*xb + yb*yb);
                flag = true;
            }
            else flag = false;

            costh = xb/xyb;
            sinth = yb/xyb;
            xpa = costh*xa + sinth*ya;
            ypa = costh*ya - sinth*xa;
            sinph = zb*rbc;
            cosph = sqrt(1.0 - sinph*sinph);
            zqa = cosph*za  - sinph*xpa;
            yza = sqrt(ypa*ypa + zqa*zqa);
            
            //cout <<endl<<ypa<<" "<<zqa<<" "<<yza<<" "<<endl;
            if( yza > 1e-5 )
            {
               coskh = ypa/yza;
               sinkh = zqa/yza;
               ypd = coskh*yd - sinkh*zd;
               zpd = coskh*zd + sinkh*yd;
            }
            else
            {
               ypd = yd;
               zpd = zd;
            }

            xpd = cosph*xd  - sinph*zpd;
            zqd = cosph*zpd + sinph*xd;
            xqd = costh*xpd - sinth*ypd;
            yqd = costh*ypd + sinth*xpd;

            if( true==flag )
            {   // Rotate about y-axis ?
               mXCoord(i)=mXCoord(na) - zqd;
               mYCoord(i)=mYCoord(na) + yqd;
               mZCoord(i)=mZCoord(na) + xqd;
            } else
            { 
               mXCoord(i)=mXCoord(na) + xqd;
               mYCoord(i)=mYCoord(na) + yqd;
               mZCoord(i)=mZCoord(na) + zqd;
            }
            VFN_DEBUG_MESSAGE("->Atom #"<<i<<":"<<mXCoord(i)<<" : "<<mYCoord(i)<<" : " <<mZCoord(i),1)
         }
      }
   }
   //shift atom around Central atom
   double x,y,z;
   x=this->GetX();
   y=this->GetY();
   z=this->GetZ();
   mpCryst->FractionalToOrthonormalCoords(x,y,z);
   const double x0=x-mXCoord(mCenterAtomIndex);
   const double y0=y-mYCoord(mCenterAtomIndex);
   const double z0=z-mZCoord(mCenterAtomIndex);
   for(int i=0;i<mNbAtom;i++)
   {
      mXCoord(i) += x0;
      mYCoord(i) += y0;
      mZCoord(i) += z0;
   }
   
   //Now update the ScatteringComponentList
   long j=0;
   VFN_DEBUG_MESSAGE("ZScatterer::Update(bool):Finishing"<<mNbAtom<<","<<mNbDummyAtom,3)
   for(long i=0;i<mNbAtom;i++)
   {
      if(0!=mZAtomRegistry.GetObj(i).GetScatteringPower())
      {
         x=mXCoord(i);
         y=mYCoord(i);
         z=mZCoord(i);
         mpCryst->OrthonormalToFractionalCoords(x,y,z);
         mScattCompList(j  ).mX=x;
         mScattCompList(j  ).mY=y;
         mScattCompList(j  ).mZ=z;
         mScattCompList(j++).mOccupancy=mZAtomRegistry.GetObj(i).GetOccupancy();
      }
   }
   #ifdef __DEBUG__
   if(gVFNDebugMessageLevel<3) mScattCompList.Print();
   #endif
   mClockScattCompList.Click();
   VFN_DEBUG_EXIT("ZScatterer::Update(bool)"<<this->GetName(),3)
}

void ZScatterer::InitRefParList()
{
   VFN_DEBUG_MESSAGE("ZScatterer::InitRefParList():"<<this->GetName(),5)
   //throw ObjCrystException("ZScatterer::InitRefParList() not implemented! "+this->GetName());
   //:TODO:
   this->ResetParList();
   {
      RefinablePar tmp(this->GetName()+(string)"_x",&mXYZ(0),0.,1.,
                        gpRefParTypeScattTransl,
                        REFPAR_DERIV_STEP_ABSOLUTE,false,false,true,true,1.,1.);
      tmp.AssignClock(mClockScatterer);
      this->AddPar(tmp);
   }
   {
      RefinablePar tmp(this->GetName()+(string)"_y",&mXYZ(1),0,1,
                        gpRefParTypeScattTransl,
                        REFPAR_DERIV_STEP_ABSOLUTE,false,false,true,true,1.,1.);
      tmp.AssignClock(mClockScatterer);
      this->AddPar(tmp);
   }
   {
      RefinablePar tmp(this->GetName()+(string)"_z",&mXYZ(2),0,1,
                        gpRefParTypeScattTransl,
                        REFPAR_DERIV_STEP_ABSOLUTE,false,false,true,true,1.,1.);
      tmp.AssignClock(mClockScatterer);
      this->AddPar(tmp);
   }
   {
      RefinablePar tmp(this->GetName()+(string)"_popu",&mOccupancy,0,1,
                        gpRefParTypeScattOccup,
                        REFPAR_DERIV_STEP_ABSOLUTE,true,true,true,false,1.,1.);
      tmp.AssignClock(mClockScatterer);
      this->AddPar(tmp);
   }
   if(false==mUseGlobalScattPow)
   {
      {
         RefinablePar tmp(this->GetName()+(string)"_phi",&mPhi,0,2*M_PI,
                           gpRefParTypeScattOrient,
                           REFPAR_DERIV_STEP_ABSOLUTE,false,false,true,true,RAD2DEG,2*M_PI);
         tmp.AssignClock(mClockScatterer);
         this->AddPar(tmp);
      }
      {
         RefinablePar tmp(this->GetName()+(string)"_chi",&mChi,0,2*M_PI,
                           gpRefParTypeScattOrient,
                           REFPAR_DERIV_STEP_ABSOLUTE,false,false,true,true,RAD2DEG,2*M_PI);
         tmp.AssignClock(mClockScatterer);
         this->AddPar(tmp);
      }
      {
         RefinablePar tmp(this->GetName()+(string)"_psi",&mPsi,0,2*M_PI,
                           gpRefParTypeScattOrient,
                           REFPAR_DERIV_STEP_ABSOLUTE,false,false,true,true,RAD2DEG,2*M_PI);
         tmp.AssignClock(mClockScatterer);
         this->AddPar(tmp);
      }
      char buf [5];
      for(long i=0;i<mNbAtom;i++)
      {
         sprintf(buf,"_%d",(int)i);
         {
            RefinablePar tmp(this->GetName()+(string)buf+(string)"_bondLength",
                              &(mZAtomRegistry.GetObj(i).mBondLength),
                              mZAtomRegistry.GetObj(i).mBondLength*.9,
                              mZAtomRegistry.GetObj(i).mBondLength*1.1,
                              gpRefParTypeScattConformBondLength,
                              REFPAR_DERIV_STEP_ABSOLUTE,true,false,true,false,1.);
            tmp.AssignClock(mClockScatterer);
            this->AddPar(tmp);
         }
         {
            RefinablePar tmp(this->GetName()+(string)buf+(string)"_BondAngle",
                              &(mZAtomRegistry.GetObj(i).mAngle),0,2*M_PI,
                              gpRefParTypeScattConformBondAngle,
                              REFPAR_DERIV_STEP_ABSOLUTE,false,false,true,true,RAD2DEG,2*M_PI);
            tmp.AssignClock(mClockScatterer);
            this->AddPar(tmp);
         }
         {
            RefinablePar tmp(this->GetName()+(string)buf+(string)"_DihedralAngle",
                              &(mZAtomRegistry.GetObj(i).mDihed),0,2*M_PI,
                              gpRefParTypeScattConformDihedAngle,
                              REFPAR_DERIV_STEP_ABSOLUTE,false,false,true,true,RAD2DEG,2*M_PI);
            tmp.AssignClock(mClockScatterer);
            this->AddPar(tmp);
         }
         if(0!=mZAtomRegistry.GetObj(i).GetScatteringPower())
         {//fixed by default
            RefinablePar tmp(this->GetName()+(string)buf+(string)"_popu", 
                              &(mZAtomRegistry.GetObj(i).mOccupancy),0,1,
                              gpRefParTypeScattOccup,
                              REFPAR_DERIV_STEP_ABSOLUTE,true,true,true,false,1.,1.);
            tmp.AssignClock(mClockScatterer);
            this->AddPar(tmp);
         }
      }
   }//if(false==mUseGlobalScatteringPower)
}

#ifdef __WX__CRYST__
WXCrystObjBasic* ZScatterer::WXCreate(wxWindow* parent)
{
   //:TODO: Check mpWXCrystObj==0
   mpWXCrystObj=new WXZScatterer(parent,this);
   return mpWXCrystObj;
}
#endif

//######################################################################
//
//  ZPolyhedron   
//
//
//######################################################################
ZPolyhedron::ZPolyhedron( const RegularPolyhedraType type, const Crystal &cryst,
      const double x, const double y, const double z,
      const string &name, const ScatteringPower *centralAtomSymbol,
      const ScatteringPower *periphAtomSymbol,const double centralPeriphDist,
      const double ligandPopu,
      const double phi, const double chi, const double psi):
ZScatterer(name,cryst,x,y,z,phi,chi,psi),mPolyhedraType(type)
{
   VFN_DEBUG_MESSAGE("ZPolyhedron::ZPolyhedron(..)",5)
   // Additioning string and char arrays takes a huge lot of mem (gcc 2.95.2)
   const string name_=name+"_";
   const string name_central=name_+centralAtomSymbol->GetName();
   const string name_periph=name_+periphAtomSymbol->GetName();
   switch(mPolyhedraType)
   {
      case TETRAHEDRON :
      {
         double ang=2*asin(sqrt(2./3.));
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2, M_PI*2./3.,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,-M_PI*2./3.,
                           1.);
         m3DDisplayIndex.resize(4,3);
         m3DDisplayIndex=  1,2,3,
                                 1,2,4,
                                 1,3,4,
                                 2,3,4;
         break;
      }
      case  OCTAHEDRON:
      {
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           2,M_PI/2,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           2,-M_PI/2,
                           1.);
         this ->AddAtom (name_periph+"5",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"6",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI,
                           2,0.,
                           1.);
         m3DDisplayIndex.resize(8,3);
         m3DDisplayIndex=  1,2,3,
                                 1,2,4,
                                 1,5,3,
                                 1,5,4,
                                 6,2,3,
                                 6,2,4,
                                 6,5,3,
                                 6,5,4;
         break;
      }
      case  SQUARE_PLANE:
      {
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name+"_X",0,            //Dummy atom on top
                           0,1.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           2,M_PI/2,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           2,-M_PI/2,
                           1.);
         //:TODO: GL Display with squares
         //m3DDisplayIndex.resize(2,3);
         //m3DDisplayIndex=  2,4,2,
         //                        2,4,5;
         break;
      }
      case  CUBE:
      {
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name+"_X",0, //Dummy atom in middle of face
                           0,1.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/4.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/4.,
                           2,M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/4.,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/4.,
                           2,-M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"5",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI*3./4.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"6",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI*3./4.,
                           2,M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"7",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI*3./4.,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"8",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI*3./4.,
                           2,M_PI*3./2.,
                           1.);
         break;
      }
      case  ANTIPRISM_TETRAGONAL:
      {
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name+"_X",0, //Dummy atom in middle of face
                           0,1.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/4.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/4.,
                           2,M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/4.,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/4.,
                           2,-M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"5",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI*3./4.,
                           0,M_PI/4.,
                           1.);
         this ->AddAtom (name_periph+"6",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI*3./4.,
                           2,M_PI*3./4.,
                           1.);
         this ->AddAtom (name_periph+"7",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI*3./4.,
                           2,M_PI*5./4.,
                           1.);
         this ->AddAtom (name_periph+"8",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI*3./4.,
                           2,M_PI*7./4.,
                           1.);
         break;
      }
      case  PRISM_TETRAGONAL_MONOCAP:
      {
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"0",periphAtomSymbol,
                           0,centralPeriphDist,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,70*DEG2RAD,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,70*DEG2RAD,
                           2,M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,70*DEG2RAD,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,70*DEG2RAD,
                           2,-M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"5",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,145*DEG2RAD,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"6",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,145*DEG2RAD,
                           2,M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"7",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,145*DEG2RAD,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"8",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,145*DEG2RAD,
                           2,M_PI*3./2.,
                           1.);
         break;
      }
      case  PRISM_TETRAGONAL_DICAP:
      {
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"0",periphAtomSymbol,
                           0,centralPeriphDist,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,60*DEG2RAD,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,60*DEG2RAD,
                           2,M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,60*DEG2RAD,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,60*DEG2RAD,
                           2,-M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"5",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,120*DEG2RAD,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"6",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,120*DEG2RAD,
                           2,M_PI/2.,
                           1.);
         this ->AddAtom (name_periph+"7",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,120*DEG2RAD,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"8",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,120*DEG2RAD,
                           2,M_PI*3./2.,
                           1.);
         this ->AddAtom (name_periph+"8",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI,
                           2,0.,
                           1.);
         break;
      }
      case  PRISM_TRIGONAL:
      {
         const double ang=55.*DEG2RAD;
         const double ang2=120.*DEG2RAD;
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name+"_X",0, //Dummy atom in middle of top face
                           0,1.,
                           0,0.,
                           0,0.,
                           0.);
         this ->AddAtom (name_periph+"0",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,ang2,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,-ang2,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI-ang,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI-ang,
                           2,ang2,
                           1.);
         this ->AddAtom (name_periph+"5",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI-ang,
                           2,-ang2,
                           1.);
         break;
      }
      case  PRISM_TRIGONAL_TRICAPPED:
      {
         const double ang=55.*DEG2RAD;
         const double ang2=120.*DEG2RAD;
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name+"_X",0, //Dummy atom in middle of top face
                           0,1.,
                           0,0.,
                           0,0.,
                           0.);
         this ->AddAtom (name_periph+"0",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,ang2,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,-ang2,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI-ang,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI-ang,
                           2,ang2,
                           1.);
         this ->AddAtom (name_periph+"5",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI-ang,
                           2,-ang2,
                           1.);
         this ->AddAtom (name_periph+"6",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2.,
                           2,M_PI,
                           1.);
         this ->AddAtom (name_periph+"7",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2.,
                           2,M_PI/3.,
                           1.);
         this ->AddAtom (name_periph+"8",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2.,
                           2,-M_PI/3.,
                           1.);
         break;
      }
      case  ICOSAHEDRON:
      {
         const double ang=acos(sqrt(.2));
         const double ang2=M_PI*2./5.;
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"0",periphAtomSymbol,
                           0,centralPeriphDist,
                           0,0.,
                           0,0.,
                          1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           1,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,ang2,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,2*ang2,
                           1.);
         this ->AddAtom (name_periph+"4",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,-2*ang2,
                           1.);
         this ->AddAtom (name_periph+"5",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,ang,
                           2,-ang2,
                           1.);
         this ->AddAtom (name_periph+"6",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"7",periphAtomSymbol,
                           0,centralPeriphDist,
                           7,ang,
                           3,M_PI,
                           1.);
         this ->AddAtom (name_periph+"8",periphAtomSymbol,
                           0,centralPeriphDist,
                           7,ang,
                           8,ang2,
                           1.);
         this ->AddAtom (name_periph+"9",periphAtomSymbol,
                           0,centralPeriphDist,
                           7,ang,
                           8,2*ang2,
                           1.);
         this ->AddAtom (name_periph+"10",periphAtomSymbol,
                           0,centralPeriphDist,
                           7,ang,
                           8,-2*ang2,
                           1.);
         this ->AddAtom (name_periph+"11",periphAtomSymbol,
                           0,centralPeriphDist,
                           7,ang,
                           8,-ang2,
                           1.);
         break;
      }
      case  TRIANGLE_PLANE:
      {
         this ->AddAtom (name_central,   centralAtomSymbol,
                           0,0.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name+"_X",0,            //Dummy atom on top
                           0,1.,
                           0,0.,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"1",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           0,0.,
                           1.);
         this ->AddAtom (name_periph+"2",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           2,2*M_PI/3,
                           1.);
         this ->AddAtom (name_periph+"3",periphAtomSymbol,
                           0,centralPeriphDist,
                           1,M_PI/2,
                           2,-2*M_PI/3,
                           1.);
         //:TODO: GL Display with squares
         //m3DDisplayIndex.resize(2,3);
         //m3DDisplayIndex=  2,4,2,
         //                        2,4,5;
         break;
      }
      default :   throw ObjCrystException("ZPolyhedron::ZPolyhedron():Unknown Polyhedra type !");
   }
   //We want to keep an approximate geometry
   //this->RefinableObj::Print();
   this->SetLimitsRelative(gpRefParTypeScattConformBondLength,-.2,.2);
   this->SetLimitsRelative(gpRefParTypeScattConformBondAngle ,-.2,.2);
   this->SetLimitsRelative(gpRefParTypeScattConformDihedAngle,-.2,.2);
   //this->RefinableObj::Print();
   
   VFN_DEBUG_MESSAGE("ZPolyhedron::ZPolyhedron():End:"<<mName<<")",5)
}

ZPolyhedron* ZPolyhedron::CreateCopy() const
{
   VFN_DEBUG_MESSAGE("ZPolyhedron::CreateCopy()"<<mName<<")",5)
   return new ZPolyhedron(*this);
}

//######################################################################
//
//      GLOBAL SCATTERING POWER
//
//######################################################################

GlobalScatteringPower::GlobalScatteringPower():mpZScatterer(0)
{
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GlobalScatteringPower():"<<mName,5)
}

GlobalScatteringPower::GlobalScatteringPower(const ZScatterer &scatt):mpZScatterer(0)
{
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GlobalScatteringPower(&scatt):"<<mName,5)
   this->Init(scatt);
}

GlobalScatteringPower::GlobalScatteringPower(const GlobalScatteringPower& old):mpZScatterer(0)
{
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GlobalScatteringPower(&old):"<<mName,5)
   this->Init(*(old.mpZScatterer));
}

GlobalScatteringPower::~GlobalScatteringPower()
{
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::~GlobalScatteringPower():"<<mName,5)
   if(mpZScatterer!=0) delete mpZScatterer;
}

void GlobalScatteringPower::Init(const ZScatterer &scatt)
{
   this->SetName(scatt.GetName()+(string)"_Global");
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::Init(&Scatt):"<<mName,5)
   if(mpZScatterer!=0) delete mpZScatterer;
   mpZScatterer=new ZScatterer(scatt);
   mpZScatterer->SetUseGlobalScatteringPower(false);
   mpZScatterer->SetName(scatt.GetName()+"_GlobalCopy");
   
   //Set the DynPopCorrIndex to the sum of the DynPopCorrIndexs (eg the sum of atomic numbers)
   mDynPopCorrIndex=0;
   
   const ScatteringComponentList* tmp=&(mpZScatterer->GetScatteringComponentList());
   for(long i=0;i<tmp->GetNbComponent();i++)
      mDynPopCorrIndex += (*tmp)(i).mpScattPow->GetDynPopCorrIndex();
}

CrystVector_double GlobalScatteringPower::
   GetScatteringFactor(const ScatteringData &data,
                       const int spgSymPosIndex) const
{
   // Here comes the hard work
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GetScatteringFactor():"<<mName,10)
   TAU_PROFILE("GlobalScatteringPower::GetScatteringFactor()","void ()",TAU_DEFAULT);
   
   // copy both the scatterer and the DiffractionData object to determine
   // the average isotropic scattering power for each reflection
   ScatteringData* pData=data.CreateCopy();
   CrystVector_double sf(data.GetNbRefl()),rsf,isf;
   sf=0;
   
   Crystal cryst(data.GetCrystal().GetLatticePar(0),data.GetCrystal().GetLatticePar(1),
                 data.GetCrystal().GetLatticePar(2),data.GetCrystal().GetLatticePar(3),
                 data.GetCrystal().GetLatticePar(4),data.GetCrystal().GetLatticePar(5),"P1");
   cryst.AddScatterer(mpZScatterer);
   cryst.SetUseDynPopCorr(false);
   pData->SetCrystal(cryst);
   pData->SetName("GlobalScatteringPowerData!");
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GetScatteringFactor():No DEBUG Messages",5)
   VFN_DEBUG_LOCAL_LEVEL(10)
   
   const long nbStep=4;//number of steps over 90 degrees for phi,chi psi
   double norm=0;
   for(int i=-nbStep;i<=nbStep;i++)
   {
      mpZScatterer->SetChi(i*M_PI/2/nbStep);
      for(int j=-nbStep;j<=nbStep;j++)
      {
         mpZScatterer->SetPhi(j*M_PI/2/nbStep);
         for(int k=-nbStep;k<=nbStep;k++)
         {
            //cout <<i<<","<<j<<","<<k<<endl;
            mpZScatterer->SetPsi(k*M_PI/2/nbStep);
            
            rsf=pData->GetFhklCalcReal();
            rsf*=rsf;
            isf=pData->GetFhklCalcImag();
            isf*=isf;
            rsf+=isf;
            rsf=sqrt(rsf);
            
            //correct for the solid angle (dChi*dPhi) corresponding to this orientation
            rsf*= cos(mpZScatterer->GetPhi());//:TODO: needs checking !!!
            norm += cos(mpZScatterer->GetPhi());
            sf += rsf;
         }
      }
      //cout << FormatHorizVector<double>(sf) <<endl<<norm<<endl;
   }
   
   VFN_DEBUG_LOCAL_LEVEL(-1)
   sf /= norm;
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GetScatteringFactor():"<<mName<<":End.",10)
   delete pData;
   return sf;
}
CrystVector_double GlobalScatteringPower::
   GetTemperatureFactor(const ScatteringData &data,
                        const int spgSymPosIndex) const
{
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GetTemperatureFactor(data):"<<mName,5)
   CrystVector_double temp(data.GetNbRefl());
   temp=1.;
   return temp;
}
CrystMatrix_double GlobalScatteringPower::
   GetResonantScattFactReal(const ScatteringData &data,
                            const int spgSymPosIndex) const
{
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GetResonantScattFactReal(data):"<<mName,5)
   CrystMatrix_double res(1,1);
   res=0.;
   return res;
}
CrystMatrix_double GlobalScatteringPower::
   GetResonantScattFactImag(const ScatteringData &data,
                            const int spgSymPosIndex) const
{
   VFN_DEBUG_MESSAGE("GlobalScatteringPower::GetResonantScattFactImag(data):"<<mName,5)
   CrystMatrix_double res(1,1);
   res=0.;
   return res;
}
double GlobalScatteringPower::GetRadius()const
{
   //:TODO:
   return 3.;
}
void GlobalScatteringPower::InitRefParList()
{
   //nothing to do, nothing to refine 8-))
}

}//namespace