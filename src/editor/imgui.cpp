#include <editor/imgui.hpp>

#include <coffee/graphics/monitor.hpp>
#include <coffee/graphics/single_time.hpp>
#include <coffee/utils/utils.hpp>

#include <imgui_internal.h>

namespace yorna {

    constexpr std::array<VkClearValue, 1> clearValues = { VkClearValue { .color = { 0.0f, 0.0f, 0.0f, 1.0f } } };
    constexpr VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    static const char FiraFontInBase85[236351] =
        "7])#######@&LF?'/###I),##c'ChLQXH##$%1S:t@rT.>XQDE)fj/1QY;99(]t:,d->>#QuhP/TTdL<wJD4JG>DW-F.$##.`,e=OJc9DZ]+##d**##DFwe=-)H$0?w'##Px$F7'vA0F"
        "b`6Rm2,]w'DN+##de7'I;T&$RId=;$.$k--,']-Gxe-R4emA;$5F&##e=s<BKW8R*J'e--(22F%;%HkE'ZaC0kt+##Wr]Y#=7=UC0=$f20j$v#B?h--f'TqL&,MSIt>_w'$Vaw'E3n0F"
        "iDwot0b]w'+LY$-?2JuB^+&,R$@k(j4gV$-'K[^I4Og^)1a&AtobO?-qmnUCVm16t2N(<-UI:;$[&S+HlS7pOR9Yuu;]$8@`eFVC3t]DUZO6)ENB=?/P+,##aetgL'8gV-+[J_&wW>X("
        "Fsk-$E'bi0OBwM1`*Ne$,QIcMU$#dNN?i--nP-JUs[?/Vg@Qe$+v&MgL)72h4URe$iu@lom81Qp5^A>#O(u(E9')dEpWNe$/4juGHd'8IPuH_&9IG]XwNOAYWfPe$VVPi^15XM_iCQe$"
        "q*+Da9(2)b#uQe$A0ZfLpga.h1nJ_&I@HfhPMNJi6XRe$u-Z(sw=Ads+;>>#67eS%,[i8&pSKe$:tof1R^sJ2I7Me$3jKG;rviG<QOMe$PEMMB14O2Cg9Ne$jSffC5XgJDrZNe$=/AGM"
        "ShA,N3KOe$fdfoReVfSSNGPe$MYE8@/,wo@a$Ne$3?gxk^nZYm?nRe$#>&5o17*T%?=:D.E'Bm&I=0Z-'sKe$=I%T.`(*Z6BoLe$t9Z87dLAs7G(Me$)*S29j-:m9O@Me$B+]>>$jB#?"
        "]hMe$liF,DZQUAPCsOe$bF'gL]j?2TPiH_&bG*gLfqk5&4J>>#iX+gL=&g;6HLE_&`mvPA='h3C9ZA>#9.pGMa$-aN@2H_&hZa;Qb;jVRFvOe$$aY5SoC:6T,=%v#5j[;ZR?3L#I)###"
        "`sj-$</D_&+VkGMs53L,I;YY#=vl/Mo`P2T&2u'/e$%##VqH0#`Z>m8`Yx+#.1=gG^/b6Dg0J.#wxCEH2Yu8Jd9/.#LaQ&JX6o-#^Vgp7*sH&#HxX;9f_xS8AfVe$t_Ve$,h[#@vv2g;"
        "FGMe$e+r3M`CuL<tfG<-@[=Z-_wGX(v(ofL>v?uu%25##$),##ghcdG`R8'FhHJ.#Rq:eMZ4$##t(B;-O]d9/tV_o,:Y`pLVY?X.J8]Y,U5kj.m)fK#DFvP0i),##K####Pj&kL$Di'#"
        "l+ST%:QIcMar&AOC@OfLwV/P]pMf1^5dVuY_u&MgUf].hv(n:dFu@lo;`wLpk>Kxkh'7L#b+bG#Vs'J#KS$K#oU%H#[-mN#@9)O#C/nK#nv.P#P,AP#ZlsL#5,lQ#l7(R#$lGN#?Tjf1"
        "-iqR#94vN#r$bT#$k)'MJQgP#R0RM0WHBU#`QxP#*8`*MH]egsNwKoe67eS%buL5&r@U%k_.K>,2g3v,?sZ.q'3ti0Ne[J1QFLxt6Ai`3[mPA4rv3v#iiKG;8K'&=Gwp`*?sMMB^9dfC"
        "RQ[q/TUZ]FmS9>G2$pc2/'8L#s#W`NUbUM9Q2goRG0NUSrYc]=6:N`aH&-AbL3glJS:6L#]gcY>=riM']U6L#sxWPAuB,d)K`drmVB#5oSx&E<$<IW$#P%h$C@7w#?(-Z$>]7h$Tu]&$"
        ",Tws$*re-%DC7w#a,>#%_(x-%@9+*$5tT'%4>OI%>4K.$`euF%cJbI%V'd/$tW7H%q%UJ%$xp#$IEM5,'AVP/@Wx`j@St;Z.C;W%;-pCMer@H)JM[&,X%_DN1QKkMhi9m]<Uw]cWKh5A"
        "1r<2CvrUVeLp=PSx-&Y-n=$M#QIJZNk9=sR[rDZ>*7)W%<tUP/Y;wSeIXI/_Fpn]G'2B;-W/T,M2,$b%#*Ki%))VP/d]X)E'PxGM@Mw],3Rv;%x-CLNjZT#c%.d;%M<^V-7YHF%+LDs%"
        "]Ws+MIK]],xWwL#X&x.Mw*'NK=F,f$$wlx'4<1S,u*.uMrQ@T.+.d;%kn=PS0-k_O3iUaNVE5%OC5H`-(KjF7Av%lk)0i]P(8B;-`)ms--BhPO<h]H%XnXj)o8=aEUJ#M#$-c4MKw=O/"
        "_uT+&I>*,85D)W%0=^V-p0kw'1S'gL(>VlMw.Vi:(r0mfOCWQ/`roMge0cMp7ed;%368Y-=L<X1l;MX(C$9H8F^d;%ee-x-<@$&M$,v4MvA#ghM2j3=XA'#&A+h;%&^BoL.j'[/$RGH)"
        "LJqYl;MQk+9$e3ON%&F.Pf6o%Ge(d-$lsx'B(tx'3PW#cp5XL#D*]],i_If$fOJ88@Fe8.ock/:m6ep7RbwA,=#C%MVIt/O?rY<-XqeU.g6At%$f[+Nt'j*ONJ*B46Li2UOi&<6B3k3="
        "ugFOM:awIOtF1^%a3t+..&tnM-&6ZG*2*NKf:3R39Bko8YbXq)g@.F%9:T)WdI[_&p)AsZR?/Q8bQ@vnh$2q7Bh`W%M4fw%Ad5.8aYMW%X+J[%Qq@r%a%/08FEWX(=tm*N$,v4ME38Y-"
        "CaLX(YnJCMtgRV')JS5oq/E)s3f_Z#KEGN'G0wV%k=v^$K7G9T#`9B+&h*6/[v8t%>i4u%1(W%&an^o/c&]m8%cMa<,l80%t(;W@U.1HD&hc_/abXB,9<1S,o1g.&ot:T.,;,J&NL`m$"
        "/LDs%I@Os'oh2%&:CBT8V7.Q0(Orr73Yv92O.Z58-&ew'Yp`qM;<X#P#X>aWXm2ZYG1)N^`mpAbJ;uL#@VI)&D9A/2xRb5f:9S)jRuDsmkZ6gq*W8@$Qd10(jI#$,,0km/Ym,_JO;u-$"
        "Ri?4&+<el+I+c:&]t$<&ug<=&7ZT>&OMm?&GSRM06X(C&NK@D&69PWM%80A&A/g:)Pb9F&r*7/(]d8T&p)S;Q2YR),Q=ixLc.4H&YnJI&.YL8,w>pgL2GFmf@W=ajX=/Tnq#wGr>UPe6"
        "Vk9*#<lTb$,Bg;-7/%Q/d<B>c.&no@]X->m%f'B#S^$iLa(#E$?SGs-]ST8M-lG<-KRx>-(Hg;-ZHg;-u``=-w*kB-oIg;-0Hg;-XGg;-q>Ab-MW:_AXYRX((+ae$K@ee$mL`e$E-iMB"
        "<+pcExDD'S'PbGM`$wLMbUpVMA?LSMw)/YMpEZ`M:kbRMpG+RM-JM5MrccrHbn<#614(B#,06tL=t;C$>Ag;-lRx>-pYGs-ggbgLX+UKMgNaMMr`POMJG%IMk5gNMw5gNM5S=RM@kbRM"
        "%D`lM;q@u-qUKsLdM`PM3M`PM&T>OMaNaMM3A([M)7lZMJvHTMZ+0VMx.2PM#5;PMusrXM*]i5MEgU1pG2B>#geV;HcZ*SRt&G_&#r;#G=P[A,xRkxuWM`t-8vNrL+pY4MA+^fLp6^A,"
        "'&cJ2:FlxuF#Y?-kBDX-_'TX([cRX(G>>F.ENP_&2I6.M:f6xkiZbSA_^mxu5G:@-IF:@-&F:@-(wX?-oADX-B@V3k(Od?g^E^_&r]ce$c@0Z5,E5g)BZIaFf3@>#9e[e$&p`w$vX`=-"
        "JM,W-[jPX(h<aM^[gwM1k:Qe$&KA8[5`>Gaf;'g2p8pxcOiuoJueC;ICUmlTQV^VI6jG8AiP2s7jx02UBToP'LQbcW,,hY?Q(6&>vn_VIk8[YQ7NkY?7NkY?_Y:2L>M;MqUnfcN(xnY6"
        "5+<&5YqeV@N;bYH:XoY664WA5@XxxYQ1]A,l4hiU20O2([;_`X/GdV@ZZ2^#a]3B-T;)=-H^3B-hoj#.=lhpLNV>OMdQ'*%e#)t-p]ERM;lL%MPC$LMDl7QMVGVPM$h4ZMc7BVM&r:HM"
        "#a%QMPQhSMp))PMU`:'%;2RA-kx_5/'####Y@=gLZ9<$#>8P>#d<UhL^<]8%A>xs;+wrc<YcEV%[xNfLWu-JUOid+Vdc9SR4AKl]3C8]bFs_%X)brCjctm@k$$(<-mmp:#$K:7#Z/j<#"
        "D+GY#NiNT#J$x%#;RsD#u[;W#3<#s-%JwqLZZA@#sP`S%+E(8I.pmc)nHG]Xb/%>YlH2290)t4]x_Pl]+j')<FR.G`76'Da+G?_>_P7SeKxj4f[#eiB'i:L#0IDfh4b>gD3B=]kot,aa"
        "SSSX#<h+u#_m/i#5c1?#s#`Z#,#mj#^6FA#CMt]#J@ol#%H6C#^_d_#k?Cn#KlAE#+-pa#5Q3p#_4pE#=QPb#@dNp#*e7H#X+od#go5r#HE4I#q[be#r1Zr#RdbI#$+Cf#%Dvr#a>UJ#"
        "/[6g#1cMs#%PEL#Cgsh#MUft#AOpM#^fGj#iBcY#a^ik##i%l#G<.[#Ru6o#l>w4$ZNZ$$8qRd#L1+*$O9n4$n1Qr#-ZD8$U?>'$DR`S%7b,T%&.;/:E'Bm&@H*W-A6sf:Rw#W6a^Z87"
        "bMTG;j`0g:r@H)<xX2&=KMJ/C;/+gC=nJ>>Qv/sQsd]MTu+eV?Ckj5&=oMm&>;@8@Rqg;6=dvPA>H$p@MSS/C3d=;$^XF0L6tbf(Sb/,N+4hB#V]#Q/^ZO<Huk@1$D+4rLXJpV-)v6R3"
        "DKtm'9g*A'R`MQN/b>0Ls191$,uW-?,G]fLd.0,M'J(&OIMW_S_:sZ>w2m-$:27@'ds5s-D+1aST,_:.xO(##6twP'(e?FR8SCf'On,/$I0DJ7+*r28T)oP',#.26VA#Q'6<R3R$8_BF"
        "+a)*W?>tsZ5xAo=:NX?M)(,h:vndiMI7$##$P6.$o5Y&5f[)tQ;w.>-HuqtLq)QZc3$oP'1E'cJ[h&CMWT]F-/.,p7vQ]5'6J.rPNbDOaKABNgHlb1$'h4Y'q.Q8.U5At6Zg6WACLVX-"
        "l&u92=twP'T^Jc$Qk5?7=9,<m9$[GME@HA8R,sc<_W_.$jgG)M_d#@N:C+8&3?Z>-=1AZ-8NYdOmrnUP`#o[/n*J88AN-V<Ic:Q';e[^NdY>W-?>J9rX8?bA)c&RjA1U,M_c>0Ld_V<-"
        "#$CY.hi,6''F/2'vd<NM(*Qt$*o'^#7?p>ICUUp7?KuR*dD&VMWBkB-g+QaOC.%:Rmfxb-1`I0HvhT8/X7>W8xqgB+132ppX#,h::_Kw^RHNH7I.%lXQ;,)OVfNBOU@Y1'PKC.?]igkL"
        "haIS'v4K%8UQ8WAkMLI8.mWpB7w8KNT<+d8j'xP'DC4H8>SapB]:sZ>)0fIM33/g'IFCgLAFlX8e<xP'%oTOMp2O/Odl7m-_5Iwp)nOu'5ZRqLRQ`::in,G.l>'##n')m8>F_Wf+&<+&"
        "mXB0$>Je['>d,p7>LmC)D863#a@6#:]i/:)DN&aNp*?wL_7U;-X[^H'Ll)>NcUa/8q(IdFPW-U&u@$Q8[tA:)Sa//Mgqs#lr+h9Mnp$.HXNo3+pccBFai:F%UZY6Sm?:@-uBAc&R,`^:"
        "6&3'mJ[;=-KZI%ZZTm'UOLXR-QOU;-KADgY8BLxA@''R]Qrs#l(em1$-q]w09RT-QiLM<-sE#l$-br8&#^MfLMOUas]/g.$AnhhLF'2hL&V>W-es4F%xZj-$?(^:)+,fl'PrY<-MVjfL"
        "hlG<-Bh:t/>l'f#-=cY#m/D9.%P'Dse8<;R)htxY(^OA>*u0^#FxCp.5Y>>#hgX_&0hX_&3pU_&0Cae$8[ae$@fx-6u3X_&'kh&#4aM:Mrdl7erk`cW$c0^#?/Yc-jq[_&Yi_e$c/ce$"
        "8f:F.^vbe$QEUX([cA/;$:ADXg[+p8rjVG<IW(5g,CH?$0',>#X8:5MTY;;?_bpA#:mG<-Oi&kM>#Vq#/=cY#Ltr4#.C8_M&vc4MS6PonUrB>#@GZ_&lp^_&fiHwT&NhPM3M`PMi+aaM"
        "nJ2R#ehJ;//O.%#hA0[M7o6juBgG<-<Rx>-'Tx>-?scW-lP^e$,+TX(C1;F.A+;F.pJVX(9.]_&NSUP/@%/Yc)7D>#A(/>cbEruP&ug>$*mG<-qg%Y-YkVe$?6<JM]+#a$2<cY#(8RA-"
        "'#)t-gq0hLfbl,Mhakl&:o>>#v`6A4>a:6/,i?8%-$ffLHqugL4Q7IMJCH>#3RwA-)sCp.(8>>#OA0eQRxOq22?-',J(-5/_i@PA)W$$$NAU2TuZoEI8Yu.:$d7/$+^[;)@g<,<9;5&>"
        "#s,v?D1:Z$wcKe$%x`e$3Mde$K>_e$vbMe$TuQS%EWCG)B1[;@`rPe$q3kSI0+4mBi$c-6uQj-$s_]YcjALgXCO>s64iZlE@SBeG7b@s6]q+30ZZX&JY6As6KFEgX.'Tv78aH&#dpc)>"
        "k+@V86$e>5ji[#@nX]h;62*^#c2(4;lXFE=8>E#$mp(F.t=4&>:Aa>$k'>uu84DMMrY6##*;G##.GY##u*f'06`($#:l:$#/hk(0B.`$#F:r$#u,`g%1*%)*RA[`*VY<A+Zrsx+_4TY,"
        "cL5;-gelr-k'MS.o?.5/&TYc232;D37Jr%4;cR]4?%4>5Eu+XCvUKV6Kn,87O0do7SHDP8go5p/8#Q509,mP0:52m0;>M21<GiM1=P.j1>YI/2?ceJ2k.l>$t=58.^5VY5+M+u88=mp8"
        "rCRFHP@E6Maan$C`HJe#s)B0Fpr0vBRDo_#KTGs-_3QtL*%+`#WTGs-m&juL,1=`#dTGs-%p+wL.=O`#pTGs-3cCxL6[b`#2)9p27U[#M(on_#2UGs-EHt$M*%+`#>UGs-S;6&M,1=`#"
        "JUGs-b.N'M.=O`#VUGs-pwf(M6[b`#p2T53tj(*M(on_#oUGs-,^@+M+.F%$$SGs-:J=gL-:X%$0SGs-H=UhL/Fk%$<SGs-V0niL7e'&$V9pP3Z#0kL)x3%$m,*Z5J]m4CrvX+$Fv.>B"
        "6uFNDl-]*$[2JuBl'(PM'M`PM-r@QM3@xQM9eXRM?3:SMEWqSMK&RTMQJ3UMHhiOF&91,$Vfj*%fLEmFlZb+$VEkjE^q11HeHo*$:>XS25@7#M*(=%$.UGs-C3O$M,4O%$<UGs-S2$&M"
        ".@b%$JUGs-d1N'M0Lt%$lVL&4eS58HPJ+%$befYBfLouG+QUG$,C5_A,)E6Jo>1G$#CAB&[<m-#b9F&#pj9'#'9q'#0Q?(#>,3)#MVs)#Y%T*#fI5+#u$),#-Oi,#:tI-#E<x-#MTF.#"
        "Umk.#_5C/#l`-0#&;w0#5fa1#?.92#HF^2#Qe53#Z'Z3#c?)4#kWM4#tv%5#'9J5#1Wx5#;vO6#D>(7#MVL7#Uoq7#^1@8#fIe8#nb39#v$X9#(=':#1[T:#<$-;#EBZ;#NZ)<#Y)a<#"
        "cA/=#nff=#x1PY#.Ml>#GF7@#Rqw@#ojBB#%9$C#1^ZC#);X3.bq0hLOOQ>#mp-qLNCH>#6Hg;-7Hg;-8Hg;-9Hg;-:Hg;-;Hg;-<Hg;-=Hg;->Hg;-?Hg;-@Hg;-AHg;-BHg;-CHg;-"
        "DHg;-EHg;-FHg;-GHg;-HHg;-IHg;-KHg;-MHg;-OHg;-%p2g:$fJGN]3,)OX3c`O[N_]P]W$#QXl:<-'U.Q-+x_5/,GY##bRtGMKL4RMN,+JM9dRIM:j[IM=&xIM7W@IM5K.IM8^IIM"
        "6Q7IM3?rHM4E%IM&FPGM/w(hLNF6##kqKHMYCNMM4F+RM`HWMM_<*mLOOQ>#(SrMMmg/NMA'LE#oxY<-s4;t-;:xNMu;TnLUhd>#mbcF#K_=oLb*w>#'=VG#/U%H#fd6QM5;>GM%@GGM"
        "0-VHM')HA#=U2w$]Qjb&+X]]&vQ-k&FT,n&#x+)3Hbv('M[QK'#qHS'F4aD'X-n&Mk1N*M=<iR'b?WuL'RQb'j6W-(7MDs':UUZ)CZAI(WMn202<(7&oQi$,FWOe$3_tZl04)78i=J$c"
        "]L3l(E_sp)$FU7);'O**T0<L)'72xL40Fx);`.u-VkniL1NLE6trRk1R+9u?,RsbE0O+dj>`;]#?Lw&PMPC7]wBs&Pa90]G/s9-NP'fu-#2Oc3ruM9S2w=J;#G#8Am<?ooIaiu?&;t=d"
        "X_GD4vfe%c=bKD=X.pXIpxX>6FYo]>S1^gLLe=m-]DA64BAT^-qMA:5QiL0.l7CkLrJq#.A#a-.jKYR.%p.A.<o-D.`Yvk.u?pY.-T;d.B>*,73[st.XRo*/4]'7/(;)D/h2.l/Oaj]/"
        "6CAi/G/rsLeY4'0Xd3M0oRHW0%GYf%WAk9&&cvdN`ctdW?iDq8UD'eWw3Lk1<+;kLR]s^luIo-</HCR9#4HkU)ZP-s?*R_>bs%Fb:/xk1i@BRT5A%FtW(nkC0@ARgR:L.3u'mkUIkPx$"
        "sX=r/o0mXI4([Xe:H6Y.Xg>(P_W:)Yix7Y.A:b@Q[`,ro51c.<_%X(Gj''fsB3p4CCFp^oU6$;S28+q]amkf*^v@ST2Is(Y*<_s-@%el71^tb7R*)h7mT#48)VCs78SA#8tAV-8&%W=8"
        "D;k&NrN^/jF>]*#UcH*5s3mN;R.Ne$oHSHX6FssoOO#U9##?v?:w8L#CwKHtiNAU'Y&:L#9mQb,ttEb5J7jTp-s$1*lT4L#PVr1;>x'4;K]V9;_T1`;)=WI;ov/R;JNwX;[@6d;3vco;"
        "s3/#<jp]+<_P54<T4d<<O3aE<]uuO<gqrX<rc1d<%].m<q?]u<R=NsLjVAW=TP`S%kOobl'B(8(>A,P;$Q+]RjZ2ohLW-oqG:p+,B5t7:(^4]R@qtUpOk:D-?ti+G%lPu].[>D6Q.t1N"
        "W/U%n$mGJ=61x1W((9Vpon-d#Xv;8Cq2M]e$&###,oQS%8)(G`v6NVH+ff(Ns2grH+Y/GM`]*DNd]IcMD<5/(HTlf(LmLG)P/.)*H1'2BO9h`*X`EA+]x&#,S'frHHit(EvR<lo5b5fq"
        "Kx9DE]$3>GB.v._](%/Ug_<GV$0iuYee*@'0Ib1$DRKk4q.hiU:111$%#GqD+x@DX_[k.$Q+qKP);v.$i&$XUk-;jT?k8m&fGF/:HB5j'Md1g(R)iG)VAI)*7t%g:;0FG`'SC8R*fDDa"
        "XQ'&b^s##cZJ4dD`ljlf%uKMgWLJPo0V1Z#4oh;$R`jYY/=UMg$SxoekO5/h3$/)j(GaVd*Fmfh0xnon8*'WHWL-aE1@f]b>KM)N_fuPST'fG`X?F)a_dB&bc&$^bg>Z>ckV;vcpVbY#"
        "jQ@>#tIX&#aj3L#iYkr-v[9^#>Y5L#@YYi9pv<;$WO6L#S)[o@%4m;%^b6L#]oo.C'FMs%%e7L#6+?]OmBSP&?^8L#akPuY>Jec)vU:L#;HW.hkN$#,5@;L#L[xRn=cN#-<U;L#TBqLp"
        "?u/Z-Ck;L#wouA#:i1$#YA4I#Y[0B#SdX.#BV`k#Bxg^#'385#UbFm#Z.$_#t1U'#Qp=:$YP>/$&25>##g*=$vclJ$^'o%#6xp>$5p(K$#3U'#Gq;@$IP%L$mU5+#9H-G$8^7L$jMU/#"
        "H5<H$Hv[L$*;e0#_F,J$[>4M$Te#3#*WFM$'KFM$b=@8#a;vR$adkM$x.,>#ZLdZ$Y9hj$7a?(#.&V_$*F$k$Wvb.#VaZc$VeQk$n]h/#iSsd$f'wk$3V31##M>f$u33l$2T]5#g*9m$"
        "c@El$sk*9#(RJx$$Ts1%gCg*#a<-(%[g82%gGC/#+GGF%&-pM%Sbg2#?X7H%99,N%&0s4#W2UJ%SE>N%7go5#c]?K%cdlN%Wp)<#5FppTt=rc%01wS%KJ=YP]XA`W5]c._^M.Se0?Oxk"
        "X0qFr+rdS%Su=;-.)``3Vp*/:%=4;?=#&/Cx=8GMTrQfU1D5M^bfn4f<2Qrm_NZ(s-.N5&bIG;-0/i`3k=DA=bQorH86JSR#KES[UgGYcx-QfhDJZrm5^@#,vmaM0LE[M9GeWSIrdJYl"
        "qSfxt+;YY#`>kZH:v=f_k?nl/OE4eH)fmxFFD]Y,SP[9MdCe'&s7lKPgbPS7JF9?-1>@A4FIe'&Bq//1I$258LXpv-5Vwx4HOe'&F3gf1M<il8NkPW.9oWY5JUe'&JKGG2QTIM9P'29/"
        "=19;6L[e'&Nd()3Um*/:lPA+FHlUS%5@c%Oi%?D<+b08@['NYG7a[j:w$SY>bEf'&<>v%=/$ho@qZZJ;LPxfLKpQF#'IiG#4;.6/s*;G#bW_pLNcPI#>g(T.KWF.#wSR:0'a*DsrDj%t"
        "2-B-m*?*a*$0K/1)5#>uSJE&+&<gJ10T_s-:bw<$j,-?$ZcUv#&VLV$WK0>$&K$(#0S:;$YQ9>$U>TkL[qD;$[WB>$WD^kL]wM;$^^K>$YJgkL^'W;$`dT>$[PpkL_-a;$bj^>$^V#lL"
        "pdj;$dpg>$2pZ(#3xq;$2`_;$>:@<$mkkgLl@kM(w5tr$>^dPAcB/vGU<ro[>7YJCkf7,MekJDWGb*)aUpH,E;t4R3@8ocM/2pl]I^6SB&V4)N:9XS%wY_PJM,>5S*/PM^Cdw%tE&s/1"
        "-8oL$$E+M$AB[<$4FWH$GXGJ$fJ4M$*WFM$.dXM$=`6k$WZU;$u[$O$K59d5.$c.#9g[D$[?MK$nvIL$H6BQ$*_5n$J=QtL%KhI$4vtM$0$3'Mj'uH$PQiK$Ka,R$r)g(M]nsK$Mg5R$"
        "t/p(Maq'I$T;JT$iVi].WJop$Vr:T.Ax%T$f8:V%BFgonR39d2ZOtcMRgA;$v8;8e<DronAS.s?enN)3)A6vGmmoVHtBTDWxZ5&X&tl]X*6M>Y.N.vY2geVZS/[SefX=<[OW&v#DHDGr"
        "QOxE@Dl1dDKlqo%T1(dDC2?/Unp<g:?l(^F&75_A>VDh$:A0u$Pe<=$Hm9F$XG-G$gldG$v;$c$]G6c$2F5f$n,fh$82Cj$>Hes$d_^>$5UPf$AJ8]$Y$V_$[M?c$efdc$r4Ed$$Mjd$"
        "1xSe$<@,f$>D_j$BPqj$F]-k$Ji?k$NuQk$S4*1%0)XrL;gipL2*47Kku1)aXKk`a]dKAba&-#ce>dYciVD;dq==5fa<AvG3#iV?hH,jK67YDNk(P)EGJS?g@)_+%aSU/#xn[-%@pH8%"
        "^F@8#ih8*#cc46#:pF6#>&Y6#B2l6#GD17#LPC7#U%.8#_1@8#c=R8#E1PY#D._+$[;A9$`GS9$dSf9$h`x9$ll4:$#D1V$X/4E$N#LF$`YHG$$SjH$FeYJ$9O<P$'n>R$5$QR$90dR$"
        "0AQZ$U;KkL]*hp$OSvv$+jDw$3&aw$TtU$%;:YA:ZhZp.Vmgi'/WgJ13pG,272)d2;J`D3?c@&4GAqul;F^)*3MYY#5)4kO=a+##*j`a-SfPq;(YbgLj2M'#+0Dt/6Sw<$q&$?$JT7R4"
        "#?H?$'KZ?$+Wm?$&c.L$mvRh$J53ROoVOjL3ef<$J_w<$.OMu&HsliTfF@/Uj[wfUB%(Z$CE'qrKu;5&PnvS&R*W5'T<8m'vJ3B#@E*VZ>h$(#@eZ(#)####/`($#vxefLF3DhLaTNmL"
        "-xSfLUwv##)MC;$ok`=-gkl-M<XPgL&fD;$(w2+N[L-##,),##EaCaM-&r>$pQRFNA^tJM18hKMw.,GMKfcgLx'^fLY=,oN9qIiLKxSfL+n(-M6sU]MZ6U*#T2()03xL$#IFhJ#MZjfL"
        "44pfL:6U*#r@f>-Upab.,),##@`/,MU$&W$`ZV&.1PCpLU:-##^@[mLCjrr$>r'g;E6[_#M?kl&:aaxt&,GYuW+xfLbu)Z#2i_;$8=wS%b92_#GM]fCj?p1pE8cq;+kZwBDrUf:2)>8R"
        ":KMPf[Z,N0Kg`S$OsrS$B;jG:o7Hv$&$&_'>NjQU.xSfL`q@G$hN<P$e[NP$(Ama&VFL]t&&5>ub&Z&4KpxSRqWC#Y6swVZ:5X8[>M9p[Dr5m]H4mM^LLM/_)(KW/bI=PAvU<loXx[q)"
        "W#[o@s@[4olg@f--1OP&'ANfL+Y/GMa(+2B@&aiBD>AJCCx,F.vwfi'Xvw.:RT5MK56d4f7/)Sn;HR8[%O/QC^-*$#7:r$#xlRfL1=4)#KO+_.)SL;$FZ5<-h5&F-a3m-Mg4$##h2xx%"
        "[ex(#i_Ds-w:Z]MgqL.&LqL4+u2*4+vIwk=<*U88k^/4O]?VhL&s)$#?foF-KhG<-K*MT.)A:r$kVSj/n?]Y#/F]2'#X='5t6>##O9F&#52ex-v-brLSq%s$'^_@-QXrt-&Vm]M5jsN2"
        "F>Y>#Kus1#+####+]hV$6jF0Mg'n##VM@+#R.Bm$=9Hu7vj0^#^o,X2?BP##Kww%#8DW)#$W_V$:u&$.4M,LMi''oL/OlcM($?D-%gi&.L9fVOw9xiLq6@'O^?+jLPxSfLx`3B-)rY<-"
        "W6s>M.<RA-,:Mt-068s7Qg'^#hR#XUgdq&$PGZY>]j)3:.uQS%[Q=wTd_7:rv+^3OCaY']m%-5/=bxZ$xn>kbm.HP/u-=d*<9m3+#b@Xhw=[XhhIP['AFcA#Ob5s.m06B#7ZEM9ghpA#"
        "m2Zd.3BP##lo#-.YV5lL3bQ'8`65F@+IO,22Z^/1)okA#XN$QNK+lo7_&M50F$'U.N),##'$s^%K6m9M)V54O*&)Ylnl>)46BfJ2ihXe$bQ$)*X::dM?/Ht7[99XUse@J11x:9&o=]`*"
        "DPbp'CbxZ$;HmPqfx&w%W,Gs-Y@=gLGZ1%M[@HA8;bO&#JqA$0R5>##m9v.#h;I#%neL3tUUkv.VI-LM[bJ(N.dY-M'3QD-T:4R-`FPh%@LZ)NW6BS8L6Ze6jw%LGNWe.M>C#K1*CC-m"
        ";c(@pB1Md40dDE-j[td8;5eM1#CNW-v0a'/XE]>e01x(#J<C.8sG-Z$TdVa-,+uJaobM:8reHLGqPFe-tBt8K]<&fNW<wx4e_g`NIqLD3p0`$#Z@3/1b%f+#`/5##T-3H$?=%u/Wp2<#"
        "VPvv$N4@m/,?_'#Oq))%%raS8ETbA#pFHQ'jHEM0ok)M-&U2E-w4T;-e9n6%.)*x'@rVeHP]$l+aB7_8.9d&6^H8fN)0l?-sZ]ZMdxSfLg^f+On6U@6M/Kw^n-cf((#7A4s<BS@A;#v#"
        "<:m5L$c9^#r^g'&Zb+(-SY.HF,H-HFpRn?9wWj-$:JCZ@7r2W@oE8##8[q^#0G0(>[QYm:)lbA#Z`?#6f9B_-):>X(WY1.$k=R>6=vrv-4@o#.B;GkOF6oeQ9hwuuWQB,.l3UhLg,S(#"
        "gtu+#/;9sL^&@A4ToA.$da3@9kO%3#u<LvL2=Mt-4Mc$MnVEUMBBS>-^F4v-?MqvLMFhD0nsxu#RiqGX@ANX(#T?(#Gf5&>'fbA#^(N;7`5M88>4k-$m[jV7FGhD4oQr.Ml:-##DE4v-"
        "`+lWMP)n##`^''#JuJ*#ocmL-$x0<.>?F>#xCF&#ReVg2/Bkc2^w]GM`Dg:dMk0.$Q_#W.0(m<-@6GDNHhDxLODo:3;p-UM#(aqSE^_qSwcR-<1.uA#to6X1W,H_&uQj-$p=pwBM>w9)"
        "<-<X(sJ#x^'/t(vs4Z<-P]$6%0ild<I:dxF:<8SRcv)B#x:,DNY59(/7W^Y#h-Oe$F9mcECE)dEo8IwBOAuu#=[klobZ1L5.kamLSslOML)Y?-3-:w-:joiLg]aF0C+wu#Wvn51p+SX("
        "KoNR0tt7L,+jZw'%>e3bk&wgL>VjfL$W1-O.UQMra#q@'sBX^ZSWMrZel^p&<98G;,kST.HM<P$p3E:/n/5##oK$iLsYqQ$.#s>&Rp*x'ImB@0E$=;$G,<A+4l/;6w-Ns-qeWRMQ<IuL"
        "nOlcM[iv9/)SL;$J5N'MmYXGNN]@Q-qhS/._XbgL'Lij$pOLTM[e?m8P*]5'DV)..%-<JMb&/s$I)lJaeeEB-OE'g-8R3q`*x-eQoJFe-,hT)3gh1hPC<o'/meDs-G3f3M/7GcMor-A-"
        "DLw&/YZaY#af:X1V#GW-.?/L,V%78%)uQ-Ha`NY5on*;?7gA>#:r8R*p2$pI(bc6<g.K_&-xtA#b5Jm+#?^$0YZJM^(a3L,vZ&.$[LQw91'dq;?C3D<>XlA#t,?D-H:up7DhjJ2-FJ_N"
        "nqlc2/ALd2$QL'#Y7,jLNGn##U9F&#:DW)#uBV,#^Y$0#V.xfLU.(p$&sE_&^wuE7+iF&#q:hW/21m<-Ldr.M:#TfL1k-AOuLH##*UsM-//g,MR7-##v'Es-TGB`Nh5RA-`>B[%BpG@-"
        "+b$B#T_&@']c8MBDNtW-=ap92]u=/Cs4Pw9g0Q_&(@]j2FHxu-^NI#MGxSfLI>8F-LHU[-p9hk+-&1#?dw>^#.Su'/h#[&#8EnkOk:3*aRuW8]?;ur$[%EW-SEX&#HuJ*#Cst.#>X#3#"
        "3>(7#(np:#xb1?#B5>##LICqDxb9^#D&Qe$C[x2D*OHMVX=GMVl.V'8L>YY#]DS?gk?^`PEB^`PSCf'/_r55L,`Mn/*I@8JW+Q8J40G@04&7/E=R=2C?_O2C[p2:)'4j6/'Bsc<)N/d<"
        "EwPX(*gZw'f]j?BU$4#7HXOX(gUuNB#xTq)m]^'/P`d'&+vwA#.k>X(Dl<X(]Y(g*_m@v$]/fv$vQsv6ve3$6WS7v5X)1I(].g,Mki&v5E9B8#n`C(M=xSfLLMN,#+rY<-(7T;-PbjfL"
        "6V%mM$%x4_ocv4_UUF_/w`_v76Vsu5I8gp9W7UB#uI_Y#LZx:Hn7[m/TC,+#-NC;$f^-x-A)5t75fL_&sJ4*Nl'8s$XDvt$LMgJjw?wiM[anI-DV?]/?R@%#KX?G$:8dE[4.cA#trBL5"
        "Nvv4A09w^#'gbY#PP5#G,_U&#FLEV&<3:x9MEH.dDqp-df4hF$cn^d%B-7@'6+HdMo1%>NDs3vLBSZL-hV3B-a>[:MX+m<-_70@';rjRMemM;$%>51OXX]<Za3^v6.uQS%5fxV.GW0T."
        "dC8x$GU^C-_bEB-=kUaNRBM.MG&=$8>lE>HPh@<-%lWR%*tg'/b?)rMF.k)#Nnmc$x4rd-D4nEIti4;-v@vpBEK$5Nu4$##%wtJ-8^tJ-4i6m%FE4<-G=n#)Z'I-d(&^E[Zcuu#x/BvG"
        "GHA_/`s+(da8=J-6,p[+4JNe?WZJM^2e@:)&m<^#9`DkrJW*COojKM^PSdq2@nMe-GKF<-LUpF-Ls4c-kuT4FXTl3Fh)rAN]X*COXKAGrVfdq2@nMe-qUpcE=lKv$g`+oMIo4c-CC)4F"
        "]#M4Fm[Qc$tQL@-Wi5&P[94O%>-9=-Ps4c--kR4F4FrVo7*IAOkg/<-=kUaN,:>GMb@-##.=Cu9)H=2_,Sv3=<h''d0-a'%/2ix=7Ak>$1d&Z.P(0Q$j[:a%FE[@'Un3XC;Z<eZWafi^"
        "#YbA#bi^:)ws@Jh2%G&#/4K_/U1VW-)d3L,i_p3+wlriK:pY^mhK3MM/.35&x8=;$@Srd**)7A402*p@^..s@_<8<-9C>HMW_D;$^P9D&;T*eZH8YY#ifEeH.(4XUR-no[19p3Od)>K*"
        "Iccf*O`*s[X.K&m_h`YTq,:kLhfEB-)j`a-<m-@'vKiM1;XdYHiH%x'udLAKsAW^H_/2N$(/018mP(a4p0%f(_rc29`Q2s@-JWfL%;Co-c;i'SP0Z&#@wHe-H8YY#IqcA#KV>8JP+#B#"
        "G[]-+LMx2LQ.Lv$VRrt-)^NjLjL_lL*7,,M8=V9.Lem(%fm=T.Je60#+xY<-Qc-x-#XgsL#oXSMv=cG-mY(d-h_Tq^PF-##J8]Y,_2jV:4.GDOS/WeH*NC_&07L^#G=2/_m)[&#hYm^o"
        ":.J?nJ4$##-[FkL#ktKMslS3O[-]iLaq?2&xlfE[F=.Y//1x(#7qX?-vI5s-Ri[+MO$YSM&u]+MVOQ>#*pSkN?R+1#+xY<-;3S>-9'A>-[WjfL#eX0Mc.#4O$@DG)ipJ4&N]qr$+iOk+"
        "fv:ppYR9MB-U)@$C^$)*v;>G2OFZ5/?FTe$25f'&sIZ5/,oF&#SW0;e#N[&#He5O+ImB@0$P^%OdT8R*cvbs-Un?:N6*^W*$iRm/Yjj1#d2d&4Ddf/.7f>1Mgs4`%hH&<-eu-F'/NP+%"
        "^g;^#Z[n92^q2g2jk@Q-niRQ-C?08%)eT)3?s0<-e*kB-urC*Nc4$##e*8^.7]:.v1>vW-/Ippp[.AS@/.35&6Ke--ZU^-6faf_/+:r:QMbT9`;4/4=q=?p.GfMqDr@?p.Op]q)3]4R*"
        ",J=fdS%K,ME<ojLJ[bH*vQ$vQG9tJ)`4J7MG$TfLd&1kL%i2Y&o;N`AY6Q_AW(%bOY)s;?:1_VRY$N,3I+aa-)Vf_A3Bt;-qNx#/?lYY#w8`X/0XUj(N/KX%]t6=-M:;$uX3KM'jgU-k"
        "se;..<DJvL4C]c;-XX&#9rL:;BFWJi(4wX/<.1vdj^A-+-IH=&RvE/M^<%pIuYGl;.l(*8m;7@@kY?_=L9$?$2a^sLQGZX$ck0E3+i_Y#j`.;64Ijl&L/17/`[LS.o0hA#CG_s-V(;r;"
        "HiY2rwAKKQX(V*Nf0p0:m%:kFi;7.ded;^Hw/VSe%CEW-2Zr2Mw2Db=`h'B#1Zm+MrwSfL<XK_MU_D;$9Ld*O<]5<->Eo>-U`xg'O9l-?)T9Djtto*JDg6)J#;3(JT#<,MU_D;$0T$QN"
        "3xSfLUU1H-@#2H-u_1H-'t1H-p93U-ook$/iS@%#@gDedi,cs-7rCeNJA`=%*cC^?/W'S%<)@cVEf'?$K>(pL]eM;$9UF%8EnbP9s#Rq`xVS(Jt/-SUtm.:NG6]38^BEX(+e&t-PVhPM"
        "&R.:P`cto&qiE$Ok$=(9o)h>$fa.u->$;'8*%rlKOp]q)3]4R*^18fmn?EX:viriK(?n>[.RdV-uZ6X-$Q?(#$JM'#h/5##?YtE$2^c8.b51/#Kl%N0^k>R$Zn6s$Z29kLo(^fLRBf>-"
        ">^>h1V.K2%?gWt$nxd+#Ep/kL_(^fLo55GMG7GcM`A[:MTurXM#xSfLJuY<-jQZ-.$6lW8xC9Z-AOB=8eP4&>e.?:&X6WV$SWjKrn/1)3<u>>#+4n?Bs5#=-hu*P<vHEX(V>(t-=#Wk<"
        "^[o^-V%1kLpUZj'EF.%#Z)n/9m4:>Z-2'Z$-]g,MA>AB.ntQ,;D$M^#GIe/1E`l5',0O9`?v1A=d26MKc9:YYW;.SeLnP(sr%Y3Fb91qS#DT;-QZY[$R2P1#]uwtL4O.U.9vi,#c?f>-"
        "Qg/,M20B/1kU]3M#-7kC?etiC1b*W-i+f-61*_H27-v9)m@).$J'vA#lrC_&Z`(.$n<$B#CeuwBMvIwB]Q.L,gIp(k6OL^#L#3R'W31.$w6=H*.Yii0..,)NSJmpo=.lpoVn3eHwWj-$"
        "Hm&kLn5T;->`/,MG(N;$MgO.#RXii0t6T;-(:H-.WEwuM>oaW[>4k-$5T6;[5269.&8#58f<#dNPNB,t)E]0#/`($#>CTk0Y:_=#fj9'#Ji8*#VGc;-.>2m&>,WF5D[G-MMv%0)QdjDc"
        "$?TO&j+eDc_Vkl]<k)F.chm`Es2ov.+)xTew.d8.PPEa*H_gP9^D_QsBcg,M;3$##2GDL,^bi;-b'B?.,[+6#Rw;edO*El]:2LW-?C(w%i'Yt?2GD/+&&Z&#B]&*#O5C/#Bs`]+,GpV-"
        "$@3X:wWj-$bCMCtJOxd<1epw'agoW7)^D7o#P#<->i1e$g-b'M)Gmt7OxVw$Q:q`E;6T,23d#K1=XuA#9OWw0g$_-6uQj-$]-#r2G(D>#&,OcM>[W/1,Sl##/`($#HOQ(%Ql@u-B2P98"
        "V0u3+F,mfXI6]ih7a,>>%SFk2B()$=-x*2_`>p;-[%Q5MRcUq7O;72ht4Ok+g78>cQgk8.;:r$#s/oD=h-I-v`Y5-vh7FkkBYR(#LF$lL2=Mt-2`U%Ma]tJMBBS>-PE4v-5To#MIkc4%"
        ";%pfL'xSfLXAm6#s*NM-BiAR.+mDA+Bd4R*-71WS<@dOgNCs@'oPmi'HmgX-T)0T1&%U[%F9Fs-<$3L@)ZEX($ocv$Hu#tLqw.s$tl_U+-<o,48Gl$M^cbp%SN);:pI#sIWk?GaDS6t-"
        "#>J,M3fM;$i$o+'h`FHM(ZT,.+)rb9KNm92J/>>#C8n+MH@-##J;R#ON$5[$9x5?.LQhSMM/HEO?(]P/O<r$#D]&*#[P)7#Rnq7#0Q/(%&6=GiSIR]=Bx#Z$LB<N1Kd=vGf>&$$Tq%N0"
        "&K=_/N@Y'#@YtE$7NC;$-TwqL.D$J$?q#-.`Dg0M<scQ9AUvv$K8<A+7o#RE'?9G;'#qM9qCKYA$rJfLt45GM$Dg;-bUu4%D'.@'::_k+xe]_/_]2]ben'_S47cA#=3EX(S9Op.2BbY#"
        "hdV5`2dcg1,`$q7)rnAc.uQS%v0Y2`[<^<(:R]5/,Sl##DN5)Mvn5BAY3$Z]J+%AM1hJ;QKDS8/mqc9Vk<&<-N3OO'PU7dXvWe5/%L4-;>Ne`AImOw^1ut_A>pPg:1lNYd_GqiL-/k>$"
        "1_7?8ID:&5FTOW-aOJk4%Ht(j&e?^#@N,`&x#Ks-@tL;=`,DjVUt6R8>9$?$8)6tLoeM;$P,A89F+@LEF+^F-kqxb-Aoq_&%3c'&3]4R*cgSSE[s1SElN*F.eDWcj,8nd;9$<f4X&%QQ"
        "j$/qL)uY@.<YtE$+<83iJ4$##/I>9'PE)UM9tX?-t(Gp)He::##ARKs=tDJ::x(5gj:LX($gQGNmvWEe^O/*<*eJFet23GeYv;Fe<GODNYlkO%r&wFeAgFX:dhm`El:eJ8Bw<JiS?\?Ji"
        "TC.<-%Y5<-3X`=-:6fBMeU4R-wn7jLMkRO:>IoJjVL4R-xBOn-W5;Q:oPgYG+mI&#^^dA#NT$LGv2wFe6RUe6iHfYG,5nrerGE^#+mRDN'xSfL)?dD-2CMAMXcrP-*xR/MuZsJ8LLu_H"
        "M6W/:A_cA#EqFnEJ8YY#V'X_&nv9sHtq)<-hv@Y-vv.F.soWcjM#&B#=(<gX6/&F-etfw7T9T;.C;$T.3r_Z#A]4_$)W_5MCAg;-,uW/%9P1_ARCr-$UsBL,%Z,]kTSa;.O'E''l<kx="
        "GarA#tse2&g34&#;H,lMXI5x'>@CQ11:gK%'mc=l7Mt^#0U`Y#?tcMBqHfX-/-c-?glv/;q(ixldXfr'XbUq71LZv$0KeqLqw.s$A?,l&cAZB=j,V_A?eT#>Slw@@:1/Z$65R^-M2:w-"
        "=^qxNn4mf(apa_&a6#6#JOeY3<05##v*^*#VI-t-xaR%#xD-(#K%vP8)idd+V'nj91V&q'mY2&N9]02^W(Y,MsgZP.V+iVH:U:^#BH*q'$SeA-lRi6&Z*79'S7UF>Axlan5.cA#fH&8o"
        "-ea<-&.7:Mw36A8:]_>$Mgtv%v@'wL,rJfLPj[I-fV2E-#m>t$3.T1p/R;6Ax-0eXPXZ##nHCYUKS=RMR@]qL<H%m-uTL_&S<NP]#N[&#[OTk+hQt.=qt9<P]b@F'`'8:;,UEW-Pf(L*"
        "Bc26&?,_V$HiM2i^0nlLtqjUM)-<mN/G#s%r2$pIKO9R*V*[La(&rI(NA5.QU.35&`*d]OsW.44%nwIMsCXc%R/[8@x_bA#1.cA#,0:JrVf*Z>+FEX('X&t-DlDuLiKHl$?og;%MKVX-"
        "dCT9T+i@(#6hKhN>f6/8:*GDOPPOv$VrX[OPU>hLfEh9&HZ<kObE%(8=3eCsk(0B#')>J:MhVW%&$9a<I64eHRc68%dB&/1Wa4eH09m.L[)+F.*.*3(I15##t#Eu$g78>ct&b-6dHG6#"
        "e)k-$?:9sL,/gfLOL-##Hl=/(xCuX;F;O2((PW/1sVS=#+YU;$,0q'%w&9j%)J?_/uQj-$:LuA#X%CRE@?<jL*)SQMls+(Naf6b<:Z0j(2(lA#2bGL%,Rw;-=m,5MgrU'%7v*$%YXJM'"
        "DkhWhxBNkF&gB>#G/B>#[jAkF+K/w$D(uf(@6KSMZh`V$$s.>-:hG<-u4T;->[:d-TKu9)A',FIdATd4WX6##XTL;$p:92#ZVkgM<VX)#,*jE-k>:@-R<8F-QBiIM*qb*E)APYup@[w'"
        "S;YY#uB[G;&35#>L?4F%JoKwg[VT-Q;Od--)gdw'YsFJ_Yjbj([V_V$0,45<'aGF>7j]VQmIPlrbbcoN)*9-#10s^M2`/%#ahtgLopIiL#8K;-&K#,%BS@['stJM'O8[`*%k$AOQ>5PS"
        "cXEcVPfHV(X:vq$331TNR,vF`uQ#^,m/eA#)4$$$/(*T%kVc'&GE_m=ICVhL=$FAF-b5,E)cb&#SlIfLmUZL-;4PG-LLcG-5_x>-OvR/MslQ##3..<$9.@<$t<9C-uwlt&5=7m8nI9/D"
        "9LuA#71Fk4$0c'&&hj-$0aC_&rD<A+4hQ_#o+SSexR*)WntNGVY#,F%Kh*4#)####e5o-#$YN1#m>S5#j=P>#lD6C#oNrG#rXWL#sV+Q#nHBU#GZuf%Ptof1kU[Y>f8HMKaq4AXYA@Se"
        "LIk.qQcoY,LE[M9G(HAFHA-/UKHPYc@V%5oI&*a*>@5s60>5qi6Et1##YlS..'r4+[0PG-+rY<-GHpV-6U'x'-sdw'av]Y#jw?*E8OC'#da)&+kE(v#*aCL.*JY##s$3W-c0i--]e-##"
        "+Pc##<kE=$t2fL$*WFM$1s'j$,dbi$2v'j$:8Lj$F]-k$R+ek$_OEl$=4q%M:C+p%jFhhLf<TnLlV1-Oq(^fL9opC4uRoC4[9hq)O/hw08#e4oGIJ#caRaD+OwlA#_#47/2-*p%$jHAO"
        ")vsG#EK82'@Yd`*=Al?-2#47/vPA2'6]c<OHR@5/c:C>#_^]w0g3Ox0UVw9)lBaY#`*M=ZO'vA#2b`'/f&p-$r[4:)TVuu#xg8A=tNM21[bniLuO3v6^n-j1GWMDFaEiP0:n`VI3M-v?"
        "A`RKu$#A+M5;Tr--NHdF>1O+3J<@v$#`tA#*0EqD$oVm/ev]w0d($##?-<s%0.lA#r.gr-2fSq)AZo6*NjO_/8x7`arruMTuK`KcUQdtLF7c`NO%S(#tAsl&*.G?-[5)=-R8GDNsbD;$"
        "SZTEn#,q;-AOuG-35)=-20,)NrF-##.OHD*d=YX8i1YD4GwR2(Z_pV.5XjJ)Hg0j(lp'pA9FCB#0*[S[$SE-d0KV#5Cu,j1FX1p/o$@p&P-^6E&[f%#n*-G*DF;=-T/C<.ji?8%iYhG*"
        "]C:)NefU5SnJT5S-99`/:O@>#sLC>#(bWr$X>^T&`>aP&b2W:O*<DG)JN,8fH>RV[<68p&#`tA#82.@KSb46#E(6<*B;F,MCsJfLHQ`'#*=p6*<,2XM[6rdM=;f)*R)gbEV&'##D5Q,*"
        "3;Mt-W=d&8:R'L5%[/nL&AxQMa>?>#A34V.ePY+%#K:@-$.%I-L'qC-pOZL-'XEr$PZAF.D;h%MKfm##bKhs.6c>>#priX(Vdf;-NKn*.[WLI9aqM*I.Nc'&d(>/Cs0S,2n`.X/M>fn$"
        "oI78%tHg`W-BM>YEORG`GX%@0tZJrKO/1[-?U=X(_^Q&#</3b0/gh;$^0Bm$Hx=M=O#>R*sp6R*QTI;Z@rYM^OWg:2Z'6R*I62#YDx0/`^@_h&wStw9n:$x9.+HAO<pd4FL9=;$+H.vY"
        "O60&b[Bt]58a-ZZ@Fnc`OA4g&T_Tq)fLB`S>rSq)QaeVZSNg]ba#a>$0:6T@0$E[$cns1#)####/`($#lUoV%Hx<X(Bk(B#clRfL]KTP#+Q*rLRF6##w$mtNeo5DOueH##q&U'#Ijb.#"
        "D4hi9^l6WAV'<$#ltZMM9@GGM'SNuN*CYgNI%<.N$:MhL+m2Z#`D@#MLqmm#2ClY#0Nd;#5xhv#VZK#$OhR%#0Lh?BphHqN5%sEO_kh`Nc)u;NY@GGMwDOJMm)mx:r0C^#no*$M]XmY#"
        "h.1>;-5_^#GN7;-d)k-$ilqg_j'mKPq(@k=VRD>#vx,j0L*d&#D`x;MhLi9.uIZD<Bt_>$G@*<-s+KV-#SGs-kav*N<Urt-Oj/+E]eHL,jkFk4TsHLEN=?>#0VZ-.7+IqLF(c(egsGT%"
        "Ff9SeX988%)5###7hv.:=Gk(E0[v:QjeuFimrK;$p)GG2q*kr?lcVfLgECYYb(0Mg]ar@tQcfY,]iCA=d>)/LkjdrZjLp.hiG=Yu'5>>#f%-.$YT&m],m(L>%(*,W<.k-$>5QxMq69Yc"
        "n`WvQiv3sQ0x33iT-Y`WRmIfh$&####dVj;4Z8;QY2FX(g(?>#,lecV=Kap$Stl]X@f#m]LXrf_XKk`ae>dYcZ)A_#tW7AFahX0L05<5AlvmA#7x37/KuXJ(^4fNMM&xIMWA:M#h2G>#"
        "?)l?-+?l?-aw37/_:P>#ZcxCOYMLM#?V_V$))l?-T>l?-4w37/L_hV$lLw#OvF-##ug4/(78`P-aH;=-f]u%MR,exO3)ZQss:::)qPEe6h7x9)P(%`/jLFrd<jDe6)'n9rvWq-6rIAX("
        "#j5.$uQ#^,4IL^#%Ti(NTRW>6jcf]GT-2p/[nt_/%R(5AU=lP0AN%j:^WA'fVbb_/'e5A4n:F2:dn=#-fOBW.T/O(jK#Lj(t9@,;J7_B#C),##$uIL$@.l?-m=eA-IR1H-%>`P-BSGs-"
        "9eUY9&Ptm'%exi'iX)^Ms=.j:F/72LXUci_nuf]GRB2'#605##d=&OP3LVGD*a)&PUK.Z$oFC[-&l]9MlWUfL>V(AM(Q5W-a;Yw00Zk;-fa4?-)',k-2MSe6.eo?TbEQR*0(1hL'%<.N"
        "67,,M]-p+M;))PMaG#59+5<gV8lUJ_@#T?&oQk&#&x[h$%$A()?CTDWKtNDac/s>$$WFm2uQe2&UTxu#6`:/_.xb@Bwdu9)LvA5]/Nm:)c'u9)Wxqf_`Sc29xlW>-ef<MG:Ys9)P8#m]"
        "6fLv0pW<n%JA+K;`60/(%uT^#o#``*9T*'%ODc>#ee39#[-n39:KpV.]:M<-+KBc.*GY##DF5-M4]Z##Ywfe?Ew9e?pRZY#qxOw9sLd5/c//?$[?frHTSoS/Q]4p/s1bM:t4He6*Ze&#"
        "-t0hLS<+p%FdAN--j&Z.9:%@#IgdD-SZdD-d8ZPM<=%pI4t/9.7fCYG@h6^GIT]#G&&0Z$Xk&Z.3nF6#EidD-VZdD-WZdD-HsY<-nH8,O0io#M%(2hLUB+p%Y]rwAfbb&#`_kgLwJntL"
        ",&(M#.@_B#A9o-#@x0J#<TGs-<2'sL#U#<I-GYY#p;'5Si2BPSSbZY#8bEYlcO@VmurbY#7Bx%kYEO,24DLe$#^cPSg5a>$2ir,*v;>G2uS*^#q*hA#*o'^#Lx`s-QlRfLOO6LM+xSfL"
        "auY<-p/)P-1ArT.75]-#L>Qp/2l:$#(,3)#1@;=-QVjfLQ=7M.WKBD3iN/<-Q9x/MilR(#r`&?$d)3c89OO&#uRG-MH:?>#+[RgLZ2'-<Da*L>k,R&#:_.u-+jAKMt-Nm$h-cf(@k_l8"
        "EF%;Hk-4'-D9(sLgwN^Mn&wLMi$vM$LM`.M;ho#MpUW,#Z.4GNp8QwL0J5s-:F@fN`9^7N5B+)#+YU;$e8/Z$.vSN-mCBNDB]kA#u9xu-4`c-N5#ZoL.J5s-Rk85:H7h>$lj?2M:I@&M"
        "M]&g%iHtxu#IJn&SQKT.*Juuu@T-T-c0e#'ZA)@'F0GY>+N?Z$GRH&MftiV$Ik*KP=RRPf@>/4+BkwP/<o-<-ul91&4L$iLd=?>#$$;P-CIg;-fAA>-RKxu-*m/KM:D%f$W@jl&k0kon"
        "KcDe-91Lk+d1:M,`r:;$70/)jT*dcrFBR?$EXN8I=XkloEP%#ZZ(9s.ah#B#fl$@'4iar?x@Hv$+Q_w-A_imLRLH##LD:@-Bk<;M>,?5&Io;^M$oiV$BWlg&^0`'8w:@k=`q+/(8&cf_"
        "kG6[9ko*m94hmP0+xtA#'%D.Qu0XT.V6GG)n*;P%=8^$9#AFeH:iCJ:k0$B#'(te?77H:VnbZc$`/fu--vsjL'h%_+6>IL>*swA#q&'82*Fl(N]k9UBKqbA#h`$n;9/G$MoeM;$pdNZ%"
        "[MYY#5r:;$Pwec`)1qigC6I?$a*3#G`-W>GVYF&#q+#-M^*36.MZbkM#Tic`tqLX(G%CZ$nA.#MhniV$`n7+'%Suc<3>a9pL:?>#=j%S<8wSR<`EcU<ldOR<lM;:V=j%S<onRR<jc`U<"
        "njOR<t-fR<3G*6//`($#_W3;NH]d-N+PM=-t/IY%P`j+MoD?>#RmRfLeff_8aM.&GSPcR*';k7D0R(DN008L#I#6X1pG2Yc,ea&Z,EQD-Q<QD-x6QD-lnsC(W_9v-:O(^#g%W-40ecgL"
        "L^=1#^Ts+MQD;&PC-+JM90AqL#pT,N.a@2NX0bO<i+-v?&Cuo]ObUq7`;xp&9+u+Dorr+MsK?##pA6L,d+va(0CjV@`jvA#%iL`])M?e6L8YY#RGt%F]ClgLm=DJ#Xll-MghNS8XY(?@"
        "q00YcDhY&#*dV2`D.]nMOtq68d@@k=^,N'fMISq2s=@Ppx8(pABoGX(>:-#Hl<kQp)5###5XJM'H]`r?W5;_o_W.L,I4%:;=ddX(qii^o.cLS.>IxlB#`tA#k#Q7Jv<<%$8%T,Mo`P2T"
        "$oC>#)')^X@['L57>gl8u7T;-oap$.d'ChL_IhkL:#TfL=r%qLKSH##.?mtL-?Ag#^d&i#?:qB#&XD8$d`mr<$['N(5>>o8k:n92a^El]H_-;-k'x5/wb`4#Cr0hLLF6##PnXoLUA,gL"
        "]Z41#jwfRJ[X4&YgtAX(UA9p[+T`S]t4OwBKM:;$+@4,Nmc/sRG#Pe$&muPS3G9/`&tvE['@1_AeKS3k?F3D<g0XMCum.ji;wpKOAr('#u'gQ->Rbg8k=J#.M>*a*_raA+c4B#,gL#Z,"
        "keY;-#/^fhOo>g:O#:-mP?4l9?t[K<3ecgLYhM;$GexGPLfRQ-2KWY.Yda1#W&9L8=RbA#IQmcE8KL>ZvG.S*=MB`NLPA>,cF#v,g_YV-kw:8.o9ro.sQRP/9A=^#-I)F.Tk320%-ki0"
        ")EKJ1-^,,21vcc2Y7qF.)2(F.kD``3;]@A4?uwx4C7XY5GO9;6j1U49[?wC#MV&E#Qc8E#UoJE#Y%^E#^1pE#V8md./b46#u9md.KPC7#a7wX.TD-(#_5Z<-nABV'#9fp'j[#WMC7/8#"
        ".s2M/VJ1v#aZV#P)d2v#tN%0:t:_#$ZbXgL^b2v#FUp[O'qs$#,;(V$)UlB$hb(C$+xRu-5X9$+Meip74htQ'm>95S-n2dD:k`'/x8$pIo=#&tnV^:;uN02'5#2c<66V`bdi/EF-IQJh"
        "Z=Jk45pB_/1Wc'&EZ_PJ%s?2KI4+:)m_Zc$&dEB-+VpZ.*GY##q:Sr%6)Mm/7cqV$rH?(%FVIq.gSY+%.VS'-VML@-2l@u-'[TsLb/m9%u;,t-t1Q=NA?\?>#R<26&LaG_&uY+44U.35&"
        "/w?8[R97O*ZeH##-tI-#Q9F&#M>:@-I6T;-2k1e$_67$9dwS,N[nYxOtCF&#'4s220&c1##Fg4N^5pfL#']iLEiYOM^mGoLJ8T;-h%kB-)(.m/ih8*#p6p*#9ucW-:aYrB6*I&%a;lt7"
        ">)LB#P*,##7XCY9&FPR*ep@L,PuH2rRwhH-wxUx$@R1B#4DWlA3:uA#[Hv223.,GM?NfYMMJ)CA9qbA#QOl)6Be=rLqkM;$w=u]:^ch>$HX4?-LAKr'.qQ[B6]NX(7R,s[MdH_A_ca-6"
        "L.3>->*EE-Z^oJNNc>d$$MK`N=sbHMswi='w(:REJg+SEr08X-Wtd'/&`,AOT[`HM4Px6'[.@R<Q,L1bP:-##$']3%`?CG)-mF,M%A?>#:%5F%&Q9Dj^OFAP9/U<-;d_D&C$^p.28elo"
        "[7t3+%)SJLjuik4Ecc#5_[<X$KsPn$8<)o$?H;o$CTMo$Ga`o$Kmro$O#/p$NWd;#-cqV$1N$(#`7s[$]V_V$$NS5#HXcG-HZ(1.AH#CDhe:&5HL<<-Jkkv-,vsjL9F7=Ev`h$';xq+;"
        "3xvuQ$Rg<HWF$?$M'PWM#E(c$^*RS[eXZV$r/7Gr15Pt:c>LF%K185#m4)mL=Owi0KNGip,9Wwt(4:&57mIV8S*)*#Fw_vu0]P>u(e68%;K#vAMo)nk@Qp$.)jhpL`;4/MJV;;$'rB#$"
        "B4pV.6LC_&;fE)#<rxV.9Nx9&oFb<6Nc8b*'E1;?IB2w%-7D,#`>$(#1%eK#6LpM#B6O]#oHLn#VCno#%'Q7$=:7w#Lk*x#18a%$[=>'$vFN-$@@p.$[j.1$.pb2$+K;S$i&$?$S=GB$"
        ":YDL)eUYZ,.HHk,]qYv,0n.x,P/($-;:9'-[Q2)-ljV)-<77--H.,3-O?F6-->E9-VL>V-w<D<-FZF>-V5:?-#YEA-Ge,C-M+.H-m6kI-I`SM-#`(O-%kcV-0$SX-@N=Y-O)1Z-crH[-"
        "n>Sa-PNBf-T`]i-NJ?o-Rklr-3*ft->H=u-Js'v-u_ax-kc='.XZ^+.`=lJ.pBi8.<Zb:.^fH<.sS^F.M4YJ.cqms.LMf*/tXL,/,.7-/:Rn-/H-b./VWK//oJd0/?oo2/$^'7/r9#;/"
        "Aqu;/h1C?/9V$@/EnKR//rXb/ObCk/.sGg1'Ck#l5/i3l%1:Zu1,x)vw+>>#[v*JGv5cd5W5brQ;Wv`*n_6J_UgGYcu02Gi3:N5&KdC,)/27&4[m@29j,:5J:b$#PIWWVQx#VYYPphrd"
        "6Z.^+5i8v5O>JAOZZ>7&)FYiUSeE&#$2F2(DHO>-uDbV7O6lc<wG6T%B.12LK=EP^TpXih2^Siqaac;%+S0j(JZX>-e_bJ2m'@^?\?0VG-_UnH-E`SM-C.L-8gr>g)Mr>^?*Vge=pFlSA"
        "+MM>Qkw`V[)C>>mjW9^#87?g)BnV)+WV)T/2/Cs7,FR2C1RG2Ua5h&#&8k2(Lat>-d=Jm0'#-aO)iL8fmWt&#Nm%KV:.D^?Oet./[jg//sVv0/F:YN/JYQ8/I:ju7cFSj0@xd[/JHE]/"
        "Qt4j/[6e-1liNM2F^OxkDYe`px(Y:v.wF:vfRW)#9K17#u[K#$GAnM0Wdj1#61M$#?NYDM9R3N0UwuJ$#mdG$-`.jM.[w10*fHS%[vJe$SGIX(cu0/#1eoCM.8>ZkCd*##sp)##oQe:c"
        "'EBodT,G_&gCMxae@tF_Ym?f^U@=xs]KU.$V4:Bm)FgK1MDQ>+N^K(rFS4``p6%@07D(##p.757lR7J3S4b@j-@$##36pLfiSpRd4mJ4nU.+`i=^J>m=1r^M2+J6MwF2io?&+##j[HG("
        "<40.$'Wl&M[Vr'#*Px`l#ADX-_FN3X0Yr'&jT3Cl;QN.#UVTIlh?0Il[kEHlUG5.M68T;-E5O<7wl^epJ1TXkBV<@jZ8EC`;2KCV_%=:GO;RXFOn@@W-X:4hnJvg/p#K*+Y2i_$8]31#"
        "%,>>#').m/<b]=#3013$0Cg;-9X5s-w;MGMsTq1M#a[lfF]DSnomLfq<4Ke$NA1V$QG:;$GH5s-c5L-M6@,gLKSX5A8(0.$0#[w'Sc?L,w8'##Bm#6&BX1T7?flAF5Paq)*q]E%$LA;M"
        "3x1p.^i'##Muiw'dVS;M%ApV-AjXe$1@v;%[@Z9M@s];QfJx;%W.OZM4m=ZMdo7K%bH#N%#.F9#:$T(Mc<O(MPw[o%Ij&X%1lQS%d+&X%ll,eQOumo%6]3CO?`1TRq3^)Woaq3+Kdi7*"
        "=[Ds%DSFv-*XRNQrPq'#<Pp`EUf%jKdF_/:S_Q##3t`cixbh5/T];>ci4KAONutcD1K'N0wrgYGIH0<p7PM=.wvai0<lnx4HkGS7Vj[i9wt.>>)Ubr?@8SfC]*EYGis=SI,&Y`jDiu%k"
        "/mioe^Y'bHov9H2,mNo[Cb<`aXp5YciuIoe.9Oxk?)LulOFxLplP+Yu(PL;$4=<5&6(W,2;L])3B+M>Ga<hfqcH-,r8ZX20[pxM0D&O]t'*UYchF-j^6,vG23pG,2vE-T7w=D8IjSlJ1"
        "F2kxtI==#53r)sH+GYY#1435&1.no%?3GJ([cw%+](9>,-QBJ1-d>G2(($29c>x.:j4m%=G>Gm/3/0DEIfOcD]TXoIf+W1p>hC]ODTs7RDNWrQRS1MTo-b(WpH#AX@r,M^@.)J_?m%Mg"
        "(:$Jh/0o@k+Gh20N*2`seaQ(sxI-s$docS%fNr:Q^tYxOhZ7VQvYKlS&)HiT(5d.Uxfg1T.Y`+V0f%GVB'q=Y64xCW>e9]XD36YY8@=`WLdMrZJW2VZP&/S[Npi7[VJ+P]T>f4]n-sCa"
        "jk;c`_%Ci^lwV(a`rb1^bLvF`sNo@bwgOxb$7pu5(OPV6)L5;6*[lr6-k18713.586Nel88NIP8o=+Pf=l=igMwmCjC::fhKkQ(jUQ/]k`8c:mdPCrmbD(Vmhi$Snn7wOol+[4oju?on"
        "$+pIqxtS.qvh8ip<ga=u6Be@t,[1cr:ZExt46I%t8N*]tD;K>#P.D8%s`6MKZui.L[rMiK<_jl&_dS`<apCPAv(=JC?_pSeYI:p[Dr5m]?iP2^L@mM^(18jTme3gUxNgDWw9kGVf1V2T"
        "qknJU0D:J_3_Rf_-mKs$B5l+`_)GYl&(/#>*=S>>It5dMeTDS[/o^o[8GBP]>l>M^'(Vi^.ptc;r@QD<dMtf:dGXJ:hM=/:6<g%b8?$p@X^K]bK-C5A96C8@;xiY>=./v>?:J;?AFfV?"
        "CR+s?@p;2BD2siBRg;/CNFAYcUi`ucW,sfCY+AVdii%8e+Ss+ML1EPJl$k(WrN,AXnt3GVpTcxX9FP`NWoq.Uk#GSIo;(5J';<JL/lScM3.5DNFmx4SVYu1TXf:MTaFn+Vb=7JUtg(>Y"
        "-qN8%8C*p%A3>/(LmUG)R57)*X`NA+uW[P/#kwl/'3ti0/^pf138i`3E=bY5Lg:,)nDcP&782)3=cIA492QG2;i*#5ZYn`*#-9/1eUF;6gbbV6in's6k$C87m0_S7o<$p7qH?58sTZP8"
        "uavl8wm;29#$WM9%0si9'<8/:tY7&+eQIVQg^erQYh*m&^*bM'`6'j')HSJ:+Tof:-a4,;/mOG;1#lc;3/1)<5;LD<7Gh`<:]HA==ld]=?x)#>A.E>>C:aY>EF&v>GRA;?I_]V?Kkxr?"
        "Mw=8@O-YS@SE:5AUQUPAsUk1gZe,Mg]qGig_'d.ha3)Jhc?DfheK`+iDvmlAhiJGDjufcDl+,)En7GDEpCc`ErO(&Ft[CAFvh_]Fxt$#G$+@>G&7[YG(CwuG*O<;H,[WVH.hsrH0t88I"
        "2*TSI46poI6B55J[tE2'8NPPJ:ZllJ<g12K>sLMK@)iiKB5./LDAIJLFMefLHY*,MJfEGMLracMN(')NP4BDNR@^`NTL#&OVX>AOXeY]OZquxO]';>P_3VYPa?ruPcE.;Q:V-&=Q9uo@"
        "ij*8RkvESRm,boRo8'5SHS,v#B/0##FGgY#TF%p%Q4s4oULSloYe4Mp^'l.qltacVp6BDWtN#&XxgY]Xxox=u&2Yuu*>Y>#.V:v#^?crdm%JSeq=+5fuUblfj9C&+_xs]+c:T>,gR5v,"
        "e5'#GxOFDNCEWVQe3uxX4MGM^N99AbkCBMg+XMci3*/Dj7Bf%k;ZF]kK0SPS[i12T`+iiTdCIJUgV*Grjpd(sn2E`srJ&AtR#=Gi3$s(j7<S`j;T4Ak-soS.uQI5/#k*m/'-bM0sr#vG"
        "jjxrHlv=8Iri2/L+Om+i'76Jh9]O&OBo4aN^J^8RS2TSRS,98RZ9t7nWq0JqdKh+rJBe>PiEquY.ZRVZ-EVYY4'=s$4oU;$jND#PPga;Qa2QYceJ2;dF@DJ_cJMVdXSw`*T;@)*$HxVQ"
        "XA#TR+C?Jh+=$/hnXnM'Y7lV-o-M8.RHiAOiPvuci]VVd#U15fvV8Ato/<Dsq/w(sKO'Z5FXV,;M'4T%GAqul>C;g:Kqn8%/[),ie6lipPsd;6[VG5S$KY8I$dqPJ/MN/L&WuSI&p6mJ"
        "Dv2mA3f/gLI3%^OI?[>PtJEp7ClsPAL_g;6?:r;$T*8W$SpRv,3([s$U0Hs6RIqxt$k:mAxK<d;HaNs-@]9AtJ76>u@s%B+(7q>,1G]2BdpOp%*TY,2aCa;?XAk8.LvO)<D*;^+Ol%W?"
        "8qFv>@W?p@76[`sUe5jBG4pMB4c:Z#Ukv>#JSV;?Vu9vPE'r]OJ<g2'tOoPA90:s?YDb8.t]Am8`m%Q8g8]294%'H)T)`,)/CJ#>@7+K(NTGj',95^=(rM&=Ca6p@&CiVQdgJ)<#cD&="
        "7%#W6Sl;v>,6^MTo>W58KKnYPeDO/:l`kJ:/TeJCi,,a<gi,d;i8cA=+=]>>TSC2TXeU29U0?s6/DpM9Z^uloZ]72p09DmA*-`2B'bGp@$_c5ATd'vl<)-K1Qg^Vm8gKj0OZB;mp^j>G"
        "`A$smlE3^Fb`;5ougN#G/(]5J1lDsHi%WPorQnAF,o@pI-Sd;He]ZSn:vwJC6Z0N0@eW&=S/von<,=gCh-R&FBqsA=^A?8n>8X,Do<7aE.o%TIFipDE8j[/C.J-ZGD'9^=)Ik;?lk/W?"
        "hEwuu+l)9%4>^]thE:/q+fds$0,BAtt83)stDj`s(2cYud3uip&Dhv#2D>>u4cCZ#7L&6&:ic##/G<jK3`sJL4_4Z>70h8@H&*QAt/7W6bgMp7q6,T@(TRs6#)4vYxT#aW&nYAX*0;#Y"
        ".HrYY@YK5]<AkS[8)4sZ<Y,m]]^9&bIPS;ZG5L>lG;hYl5BOAkrX^Mge>mYck]M;dq1fSeq+J8e-C?/h3nVGi968)j7nvfh/8:Q/-9k20rZK>ckR#?,okYv,wOF5f%i'mfYJ5pRCq_AO"
        "T#tVQ7*QSeTJql&r+TV-pc1I$xkg>$#<^;-/GpV-QU.F%Am0.$pNonLFStfMcpn`ED.9v-&og>$xO#<-EGpV-0H0F%E#1.$dZY)MJlBgM@6W/:HRP8/&r#Z$J[lS/6rj-$&$g*%DWajL"
        "u1D'#-4N'MO4qgMp:%pIM*.m0'%?v$xVJ216rj-$.s$qLRF6hMg?9>cPE*j1&og>$*N#<-QGpV-;j0F%N5l-$+%%##H?4F%[f1.$/#.qLaEaiM`TrcDvm3wp%+Hv$SfG<-_>T;-L$iH-"
        "r1^gL$9^/1b4N;7&og>$?N#<-cGpV-,8-F%d+;.$#w+F%all-$Lut228C_l83ls-$6Z,F%h7;.$:g,F%exl-$P+u22<hv.:3ls-$JA-F%lC;.$RI,j:7xs-$Xl-F%nI;.$Zr-F%oL;.$"
        "]x-F%l7m-$IL:RNlBonL4c/,MnT#oL*8T;-vM#<-p5T;-q5T;-t5T;-vAT;-'N#<-wAT;-TR`.MK.AqL'#ZoLv(doL:qOrLw.moLx4voL@?1sL'`@2NFdhsL+].mM(fU`j);dV@&og>$"
        "^N#<-*HpV-9e3F%*(3.$V]#lL/uRmMo2ioe-`%pA)4d;%4$T,ME9T;-[gG<-+6T;-,6T;-`6T;-1BT;-kN#<-2BT;-oN#<-/6T;-06T;-u6T;-5BT;-)O#<-6BT;-,O#<-7BT;-8O#<-"
        "8BT;-;O#<-9BT;->O#<-66T;-76T;-@7T;-<BT;-LO#<-=BT;-U4KV-?N#<-V7T;-@BT;-`O#<-ABT;-gO#<-BBT;-lO#<-?6T;-@6T;-s7T;-EZlS.H->>#&M#<-GHpV-c;,F%G*4.$"
        "P@a'ML#[pM&NP,2Jf:5K&og>$SO#<-KHpV-:`-F%mD5.$qTxnL0Ja+MhGK>Gtk),W'x,Z$cO#<-qHpV-ON4F%qP5.$cTP)Mvp%uM$C>,M^]2xL*>pV->o-F%tY5.$JmjjL#-AuMPpV;Z"
        "wH=AY'x,Z$LM#<-xHpV-F*,KENB95#P]7+M[2]jL+#rD3)$UYZ&og>$>O#<-&IpV-2J-F%&s5.$0ww#M+^4vM$<pJ1)<mr[(+Hv$RM#<-&7T;-xo,D-PM#<-+@T;-Zr-A->3^gL8C(T7"
        ".jIP^'x,Z$JM#<-/IpV-(00F%/86.$*95F%5rI_&xh^>$6Bei_6rj-$>1'sL6JCwMH1hxt4JBJ`&og>$^M#<-5IpV->r-F%5J6.$]1mlL:chwM#=5,MmMu$M*8T;-(M#<-=hlS.%####"
        ":CT;-&M#<-;CT;-,M#<-<CT;-'M#<-=CT;-+M#<->CT;--M#<-?CT;-3M#<-@CT;-AM#<-ACT;-;M#<->7T;-?7T;-:5T;-DCT;-TM#<-Sn/,M#OI&Mcwf,9wPO&#V$T,M3E`hLM-'?N"
        "dgjmLN30?Nk;TnLO99?Ng#0nLP?B?Ni/BnLQEK?NpY,oLRKT?Nj5KnLRHB$NvjEm/PG<JirdVG<QPWfi8bPGERYs+j4=9/DSc8GjP`Scj^2p;-d$T,MBKCsLZ&H@NslGoL[,Q@Nb]d)M"
        "]2Z@NX%buL^8d@Nk(5`8:IL]lZKCAPWIhxlav;;R]_->m[H_]P^hHYm`mvuQ_qdumb)WVR`$*;nh`OPTa-EVnv3x%Yb6arnpR),Wc?&8oTn43)eVZ)M*8T;-A$T,M^9T;-2hG<-c7T;-"
        "a?\?a$cd(*M&8T;-lN#<-jCT;-JO#<-kCT;-QO#<-lCT;-MO#<-mCT;-OO#<-nCT;-VO#<-oCT;-PO#<-pIpV-1D-F%qTA.$Tb3F%rWA.$*J/Dt7xs-$mT4F%pEs-$[*MF7vp4F%vRkA#"
        "(e)'%jll&6wt]+M)YlS.,;###44p,MNq78%;IGb%uQj-$WviuL+CYcMJLvu#6(9.$S^DuL-OlcMu$g+M[:#gL*>pV->)1F%'t/.$+l'hL,b1dM5;LlSW[F_/uIO&#8fG<-+GpV-HG1F%"
        "+*0.$/.LhL0$VdM9`d.U.h7p&xIO&#2M#<-/GpV-FA1F%/60.$2@hhL4<%eM<%a+VW[F_/uIO&#=fG<-3GpV-MV1F%3B0.$<'niL8TIeMF*q=Y6ZgJ)xIO&#<M#<-7GpV-P`1F%7N0.$"
        ":qZiL<mneMDn9]X:))d*xIO&#CM#<-;GpV-Wu1F%;Z0.$7_?iL*JpV-U1I_&=a0.$AEEjLB;OfMKWMrZ@`w],xIO&#FM#<-AGpV-Z(2F%w0p-$CQWjLBG)qLMj.S[H:9v-xIO&#HM#<-"
        "EGpV-].2F%E#1.$FdsjLJlBgMP/+P]HRP8/xIO&#KM#<-IGpV-`72F%I/1.$QP,lLK?DiL]FrCaP-iP0xIO&#UM#<-MGpV-kX2F%M;1.$J&BkLRF6hMTSBi^?-ew'uIO&#ZfG<-QGpV-"
        "l[2F%QG1.$M8^kL*JpV-geI_&SM1.$Hp/kLXkmhMY+vF`p4)F7uIO&#^fG<-WGpV-ph2F%WY1.$UiPlLYh?lLoEx79iQbA#;(k-$WuclLl?WiMe9Kuc?-ew'uIO&#dfG<-_GpV-w'3F%"
        ";(k-$/;&:)iw]w'uIO&#@O#<-a>T;-+12X-GXE_&uIO&#BO#<-c>T;-;6S>-v1^gLi^c7efXfS8wCF&#,xkA#x1^gLl#`4fhkF59wCF&#,xkA#$2^gLo>[1gj'(m9wCF&#FmD2:6rj-$"
        "BMZ&MoD5kMm,%PfmB$j:wCF&#P9mA#)2^gLqP<igoTZJ;xIO&#qM#<-pGpV-3_3F%;(k-$ft7nL#jlkMtl8fhs#sc<xIO&#pM#<-tGpV-2[3F%tX2.$#$5F%$=F_&uIO&#WO#<-wGpV-"
        "Y$/F%wb2.$Vqf(M&>VlMKVDV?$d0#?xIO&#_O#<-$?T;-1%T,M:>pV-I>%:)&r2.$x51pL*JpV-DTK_&(x2.$%HLpL-i@mM3*uOo?-ew'tCF&#9%aSA:(k-$]?G)M0%]mMNr@S@.i@5B"
        "xIO&#bO#<-/HpV-gK/F%/73.$dj1*M?L,hLu$g+M>(8qL.>pV-HH4F%2@3.$S+Ok48$G_&uIO&#hO#<-5HpV-sp/F%5I3.$p]I+M:bbnM`ae%F8nPGExIO&#sO#<-9HpV-kW/F%GLk-$"
        "h,V*MB$1oMbsE]F6(9.$oV@+M6TgoL_WI`EBZIAGxIO&#rO#<-?HpV-qj/F%?h3.$nP7+MDHhoMu$g+MB3urL*8T;-#M#<-CHpV-'90F%D$=.$)3,F%Dw3.$BLTsLIg?pMqO3MKGJ>8J"
        "-RrS&HSYSJxIO&#MN#<-IBT;-/M#<-JHpV-,H0F%K9=.$.B,F%_p4.$&>xfLI5>%Mu$g+M0iJ+M.>pV-B51F%va8.$mHxnL%:Y(N)cL%k=^m`*xIO&#,N#<-;GpV-D<4F%;Z0.$,s6qL"
        "@/=fM:j2fqYAu`4xIO&#9O#<-1IpV-+13F%1>6.$DD-&M7PLwMo)2sd>OR]c)4d;%)O#<-?IpV-h:2F%@l6.$+XI#MSNK$NXc0m]QPWfi)4d;%%O#<-RIpV-j@2F%RL7.$A`Z-HX0K_&"
        "&4d;%<$T,M<e1p.07)##VNj-$H&###OG1F%#h/.$m4>wL%.gfL#:#gL*CdwL+[(dMDXfGV):Z;%)4d;%hN#<-*GpV-MA1F%+*0.$pFYwL0$VdMA=jJU.h7p&)4d;%mN#<-1GpV-Yf1F%"
        "13k-$9w1_Ati1F%Dmk-$McD_&vo1F%SDl-$]:E_&xu1F%d(2.$Q<HtLk,gjM([lGM=E.&G#V'^#L;S#$P;#-MoS#oLB<UoMY%Qf_@a*#Htv7)=AjE>HA#'ZHV7=,MY;(sL*>pV-kR2F%"
        "D$=.$uk.F%G-=.$Zt3F%Dnn-$Ftn-$rIm-$J34.$vsOoLO5wpM6>CPSck%RN&4d;%hVu)N+whDNWn/,N$]0^#6$T,M.>pV-_?F_&Q?o-$'p'##a42F%X^4.$(U[#M[uWuLX%buL=af#M"
        "bO,8Nfs&nL_1tuLk<ZwLv[^vLqHmwLsmMxL*>pV-_CI_&'GI_&/G2F%vVp-$x]p-$l7m-$'#?.$hC.F%Suw-$qwE_&UORYdN3I21>hjudihbP9C'0;el-_M:D0KVen?\?/;E9grek$C2:"
        "FB,8foHZJ;GKGSf$]0^#9O#<-D7T;-E7T;-%6T;-F7T;-*/OJ-:O#<-I7T;-J7T;-'6T;-OCT;-(N#<-PIpV-lU2F%QL@.$#x.F%QI7.$16X$MWj,@Nv(doLWgp$N^Ii(aUuo(k%aK>?"
        "V(5Dk$]0^#7O#<-WCT;-&N#<-XIpV-pb2F%Ye@.$'./F%e$s-$h-s-$)rm-$lB8.$9gK%Mr[oBN-x.qLrX]'NTB+T%p`2Gs$]0^#>O#<-qCT;-2N#<-n7T;-CL%f$qY7+M#cA+M/.AqL"
        "rhJ+M4:Ul8>OZ?$'G:;$%,>>#ON#<-$GpV-Yx1F%$k/.$COTsL)OlcMAO+DW'($Z$#SkA#pN#<-(YlS.,,>>#sN#<-'5T;-0fG<-nN#<--GpV-Vo1F%-00.$SZ;uL20idM&[L`N0$oP'"
        "#SkA#cN#<-1GpV-JJ1F%WS###YDOw9D00F%26k-$;,D_&MK0F%4<k-$?8D_&QW0F%;Z0.$RT2uL=j@iLAD4jLjiEuLFJ=jLCPFjLhOKvLKrKgM7Jq1TI[lS/#SkA#fN#<-F5T;-hKx>-"
        "hN#<-LGpV-MS1F%L81.$gx=wLQ@-hM;o2JUO<eM1#SkA#rN#<-L5T;-02RA-wN#<-RYlS.X5YY#)M#<-R5T;-`4`T.7G:;$T5T;-`fG<-6M#<-[>T;-#FsM-bfG<-vX`=->M#<-c5T;-"
        "$;#-MZosmL*>pV--D-F%i72.$F^ajLk#0nLh)9nLiotjLpJ>kMv'^fLeFA,;6rj-$T)=kOs'F_&w[0^#OM#<-pGpV-8f-F%m:m-$P8a-6[u-F%o@m-$x6F_&YY###WX2.$Y%dlL#,;lM"
        "TKbP&wG4&>#V'^#E,o>$32^gL&W1)3#Zk]>w`K>?\?ObQj$6uoL+;)pLV=UkL+].mM,8*#5);dV@$]0^#=M#<-*HpV-rf,F%*(3.$CKEjL/uRmMvv8/1-`%pA#SkA#$O#<-.HpV-@(.F%"
        ".43.$v'rxL37xmM1f]V61.=2C#SkA#&O#<-2HpV-B..F%2@3.$x3.#M7OFnM3x=875RTJD#SkA#(O#<-6HpV-D4.F%6L3.$$@@#M;hknM54uo79wlcE#SkA#*O#<-:HpV-F:.F%:X3.$"
        "&LR#M?*:oM7FUP8=E.&G#SkA#,O#<->HpV-H@.F%>e3.$(Xe#MCB_oM9X629AjE>H#SkA#.O#<-BHpV-JF.F%Bq3.$*ew#MGZ-pM;kmi9E8^VI#SkA#0O#<-FHpV-LL.F%VW4.$QN)uL"
        "[(9rMcu3&+YB(&P$]0^##M#<-ZHpV-@)1F%Zd4.$vx[fL`@^rM1gfrQ^g?>Q#SkA#W#T,MF>pV-jfC_&_p4.$FbpsLdX,sMWg^M'b5WVR#SkA#LN#<-cHpV-iJ,F%c&5.$,q3$MhqPsM"
        "='NJ:fYooS#SkA#2O#<-gHpV-NR.F%g25.$.'F$Ml3vsM?9/,;j(12U#SkA#4O#<-kHpV-PX.F%k>5.$03X$MpKDtMAKfc;nLHJV#SkA#6O#<-oHpV-R_.F%oJ5.$2?k$MtditMC^FD<"
        "rq`cW#SkA#8O#<-sHpV-Te.F%sV5.$4K'%Mx&8uMF#CA=v?x%Y#SkA#;O#<-wHpV-Wn.F%wc5.$7^B%M&?]uMH5$#>$e9>Z#SkA#=O#<-%IpV-Yt.F%%p5.$9jT%M*W+vMJGZY>(3QV["
        "#SkA#?O#<-)IpV-[$/F%)&6.$;vg%M.pOvMLY;;?,Wio]#SkA#AO#<--IpV-^*/F%-26.$=,$&M22uvMNlrr?0&+2_#SkA#CO#<-1IpV-`0/F%1>6.$?86&M6JCwMP(SS@4JBJ`#SkA#"
        "FO#<-5IpV-c9/F%5J6.$BJQ&M:chwMSCOPA8oYca9.D)bJ-Hlf:+;Db$]0^#.N#<-;IpV-+C3F%;]6.$+gqpL@1IxMrVEig>OR]c$]0^#0N#<-?IpV--I3F%?i6.$-s-qLDInxMti&Jh"
        "Btjud$]0^#2N#<-CIpV-/O3F%Cu6.$/)@qLHb<#Nv%^+iFB,8f$]0^#4N#<-GIpV-e?/F%G+7.$K+N'ML$b#N]?DGDJgCPg#SkA#QO#<-KIpV-nZ/F%K77.$M7a'MP<0$N_Q%)EN5[ih"
        "#SkA#SO#<-OIpV-pa/F%OC7.$OCs'MTTT$Nad[`ERYs+j#SkA#UO#<-SIpV-rg/F%Sqk-$QO/(MO?DiLcv<AFC9ew'vRkA#mWu)Nd)X]F]Fl%lxLbA#ZC1Al6rj-$9fErL^5Q%NuwSfL"
        ";Ojul6rj-$:lNrL`Ad%Nv*p+Mv,q(M*>pV-wv/F%^n7.$CB=_/^9,)M*>pV-x#0F%`t7.$Wtf(Me`;&NiV5;Hc?&8o#SkA#^O#<-dIpV-$*0F%d*8.$Y*#)Mix`&NkilrHgd=Pp#SkA#"
        "`O#<-hIpV-&00F%h68.$[65)Mm:/'Nm%MSIk2Uiq#SkA#bO#<-lIpV-(60F%lB8.$^BG)MqRS'No7.5JoVm+s#SkA#JN#<-pIpV-gD,F%pN8.$_HP)Mukx'Np@IPJs%/Dt#SkA#eO#<-"
        "tIpV-+?0F%tZ8.$aTc)M#.G(Nw0#,MnoS+M*>pV-GB4F%wd8.$twH0#X:FcM8T?ip#YbA#$]0^#LN#<-$GpV-IH4F%$k/.$Iq,tL)OlcM:gvIq'($Z$$]0^#NN#<-(GpV-KN4F%(w/.$"
        "K'?tL-h:dM<#W+r+L;s%$]0^#PN#<-,GpV-MT4F%,-0.$M3QtL1*`dM>58cr/M3-v#_j-$N9ZtL76rdM?>S(s1-4m'$]0^#SN#<-2GpV-P^4F%2?0.$PEmtL7N@eMAP4`s5QK/)$]0^#"
        "UN#<-6GpV-Rd4F%6K0.$RQ)uL;geeMCck@t9vcG*$]0^#WN#<-:GpV-Tj4F%:W0.$T^;uL?)4fMEuKxt=D%a+$]0^#YN#<->GpV-Vp4F%>d0.$VjMuLCAXfMG1-YuAi<#-$]0^#[N#<-"
        "BGpV-Xv4F%Bp0.$Xv`uLGY'gMI=6##E7T;.$]0^#^N#<-FGpV-qh2F%F&1.$pT+oLKrKgMlvLoeI[lS/$]0^#*N#<-JGpV-]&,F%J21.$]8/vLO4qgMMbM;$M*.m0$]0^#bN#<-NGpV-"
        "_,,F%N>1.$_DAvLSL?hMOt.s$QNE/2'oK^##/CG)Swt-$Xt](M3tLdM3_&;mm^$2LxIO&#CN#<-M?T;-KBRe-QvG_&uIO&#@N#<-O?T;-V0ho-S&H_&uIO&#BN#<-RHpV-Z#,F%k>5.$"
        "'T_pLn@8'No=RlJl;q.r'l9B#K;ou,xXlS.->uu#5N#<-*YlS.45YY#DO#<-.5T;-GX`=-NN#<-65T;-OX`=-YO#<->5T;-YX`=-VN#<-SVjfLU&PonRWaJ2U5u`4((IJVZJ:&5^(N;7"
        "G[g%uc=jV7e7Eeb]x:2:tjvf;FBwV%s#sc<vV0#?6Dbi_&vgY?%fK#$3M#<-(HpV-mS,F%)%3.$27LhL/uRmM_M`G)TEo^fM1JqLw4%&M4wfN-@gG<-?fG<-96T;-4ZX_$XtXrLfP05<"
        "riE#-EgG<-pSVe$]6(sLH?1sL$hDxLEE:sLBKCsL2go#MGQLsLDWUsL>Y1%Mv&80#BahsLNen&MMv-tLPJntLIE:sLUPwtL&bfo<YiE#-bgG<-,&vhLTKgoSqO-/V'v6JrnLHJVq*]`X"
        "ax<Ji>%RwB+SI_&JYD_&$dp-$^P^ebq6*8]6rj-$U,=kO/`I_&xeK#$BN#<-,IpV-'00F%-26.$<KH-d3lI_&w_B#$a`/2_5;FM_^d_ih28bi_/5'/`P/%2h4JBJ`#V'^#QPbf`6rj-$"
        "Wa)uL9]_wM,7*>l7f>Ga(.Z;%O^aca6rj-$4@W9`=4J_&&4d;%aN#<-:IpV-E51F%:Y6.$_A8vL?+@xM6;15S>OR]c$`B#$i&pxc6rj-$t?3-vY%i%ME.r%M=:-)MB4%&M@@7&Mnu2*M"
        "Hb<#N/E*^OFB,8f)4d;%ZN#<-GIpV-C#1F%Doq-$qK*##[x1F%M=7.$uqUxLRHB$NOs.8[bciQjnU-F.,TC_&N7r-$W-K_&&*,F%TR7.$ZsDuLYs,%N2a&ZPW1P`k)4d;%^N#<-T7T;-"
        ".A:@-=O#<-X7T;-bhG<-0O#<-_IpV-un2F%_q7.$7w'kb$<6)MfD?)MUQrhLcJH)M`PQ)M;E`hLhrV&NZ)H/(fZx4pgpbPpgmwVQhmXlp)4d;%bN#<-j@T;-<,MP-'4^gLvu8JhmD6Jr"
        "%fK#$OO#<-oIpV-/I3F%oK8.$nZuQWu/L_&+B+RE0H,F%oBs-$Ds+##&,-F%u^8.$eYJvL$4P(N2HeAOJ#Z;%&7vV%%q/W-1dC_&#og>$tM#<-4GpV-&+3F%13k-$1En>$?O#<-8GpV-"
        "/E0F%8Q0.$DMH&M:W%iLQG>l97qa>-M1^gLAGScr>M@&,;J[A,9h*W-DGD_&xh^>$MBh>-S1^gLpTa>5D.9v-&og>$EN#<-HGpV-ra/F%QG1.$l08nLJgF?-RrK`$+bamL.>pV-<l-F%"
        "l@2.$^8&vL@0CoMx,K/1AjE>HB5#sI=[*W-K^G_&%+Hv$nM#<-G?T;-?,MP-Z2^gL$@,gL7a;5KO%VPK4W0W-PmG_&uLbA#9N^2Lxf+)OJY)d*W0GDO'%?v$q*g`O6rj-$7L_hL].BrM"
        "T?o8%ZKCAPWH_]Pc3/W-(JI_&$x,Z$gO#<-'IpV-J?4F%0;6.$i#2*M5D:wM<mdip5S^f`+:QZ$5POV-$GpV-A*4F%.30.$]6,)M8TIeM/JE#5UM+_J?^k-$^<5)MKY@Q-@>pV-GLE_&"
        "7Ek-$br###D34F%>d0.$aNP)MGY'gM#3T/1F@pV.%fK#$eO#<-PGpV-B%.F%rIm-$Sw^LEkNYA>/YD8Ad.nY6,V`SA&og>$YM#<-WHpV-I;1F%WZ4.$cSJvLaFgrMvkN2K_pZYQ(+Hv$"
        "DN#<-`HpV-,60F%`s4.$H_TsLbC9vL5iq2#+w#tLgkGsMwtjMKePSSS(+Hv$EN#<-fHpV--90F%f/5.$Ie^sLwmMxLtsVxL/3ItL(B8#M'TS#MV>[tLYs,%NXhK2BW1P`k'x,Z$-N#<-"
        "X7T;-$Nx>-NN#<-_IpV-Av0F%[_r-$ro<R*$/-8o6rj-$QKM,#v<GcM]3Q/C$c'^#(+Hv$dM#<-%GpV-h;,F%%n/.$,aUpL*UucM.>nA4(1?v$(+Hv$_M#<-)GpV-jA,F%)$0.$&;ofL"
        ".nCdMnwl;-,UV8&(+Hv$'M#<--GpV-c-/F%`?l-$A3eiL60idMKIQYu0$oP'(+Hv$(M#<-1GpV-E+.F%2?0.$aC)mL7N@eMua*Q/7d,g)]3Qt1)luG*6rj-$qS%+M>#+fMI7qxt<;`D+"
        "(+Hv$bM#<-@GpV-=i-F%5ATm0a82^4Z2#d3(+Hv$-N#<-WGpV-Xj4F%WY1.$pH]nL]-<iMDZoG;dF/s7(+Hv$DM#<-d>T;-Hh_d-lbE_&$x,Z$sO#<-f>T;-@1PG-#2^gLJ@6>uitbP9"
        "'x,Z$pO#<-jGpV-xl,F%j:2.$SJ^kLoD5kMk[p>,mB$j:'x,Z$oO#<-nGpV-jB/F%-Up-$:*(kbM(fw'%+Hv$;X`=-/HpV-),-F%0:3.$Q>KkL5C4nM&K>g1O=niL(+Hv$%N#<-PHpV-"
        "d0/F%M3o-$x`'##+2-F%O9o-$X/H_&q_.F%%j'##]5H_&=p,F%VW4.$&<uoL[(9rMT@xS@j(12U(+Hv$:N#<-pHpV-`$/F%pM5.$+ZLpLujrtMUI=p@s$&)X'%?v$Z6FDX6rj-$pMr*M"
        "x&8uMf.=a*v?x%Y(+Hv$0N#<-wHpV-i>,F%wc5.$dDr^o'GI_&%+Hv$%M#<-$IpV-`#,F%$m5.$GWEjL)QxuMN_v>#'*6;[,Cmv$``08.xFpV-5S-F%#h/.$$3loL(IccM:XqP8&u^>$"
        "(+Hv$]N#<-+l1p.`u'##*t/.$Wg;uL5B.eMH)1a<<m6w^Adk-$gh<wT`;oiL.>pV-lG,F%Bp0.$/rkgLIf9gM3mS;Q$>T9VAdk-$+^UpLOrKgMS:oS@I[lS/'x,Z$'N#<-JGpV-f9/F%"
        "L81.$'E1pLWfjqMp6`8.UtfcN'x,Z$1#T,M^?pV-KXE_&h55.$gh`mLjt,wL=Ce3#C[MmLoE;tM?-<j9mC-/Vj@HJVxEG59oUdfV(+Hv$6M#<-l6T;-+Z`=-4M#<-q?T;-,dDE-wgG<-"
        ".Z`=-wM#<-uHpV-oP,F%u]5.$2.1hL$3JuM]2Hj'xQX]Y'x,Z$xO#<-#IpV-[n.F%#j5.$ug4oL(KouMI2L&=FB,8f'x,Z$0N#<-Kn1p.0<&##%b/.$38IqL#xSfLLF6##<5suL%.gfL"
        "x3pfLuGgnL*UucM#:#gLaAev$6rj-$CB*jL,b1dM4xxV6*CvV%'x,Z$FM#<-+GpV-_$/F%+*0.$0&.qL0$VdM<lZMT.h7p&&r#Z$0Y+W.@1^gL9OU580$oP''x,Z$]N#<-1GpV-@v0F%"
        "?.#W.$x,Z$kfG<-3GpV-OL.F%3B0.$l3AnL8TIeM@9jJ:6ZgJ)'x,Z$jM#<-7GpV-mN/F%7N0.$bMAvL<mneMG#(a<:))d*'x,Z$dM#<-;GpV-T[.F%;Z0.$`A/vL=j@iLg8#&#Rd4oL"
        "B;OfMLPZ>>@`w],'x,Z$ZN#<-AGpV-KA1F%Am0.$3BB2#C`tfM<kQ29D.9v-'x,Z$bM#<-EGpV-E..F%E#1.$22@qLJlBgM=tmM9HRP8/).?Z$x8h(E`YX&#NaK_&xd/.$`:mlL'CYcM"
        ";a62p%lB#$)4d;%lM#<-&GpV-j?/F%&q/.$f_MmL+[(dMZt^2B):Z;%)4d;%fM#<-*GpV-g6/F%*'0.$u/.kXKhPgL*>pV-h9/F%,-0.$dR;mL1*`dM1[&vl/qR5')4d;%UM#<-0GpV-"
        "6P-F%090.$_4dlL5B.eM3n]Vm3?kM()4d;%SM#<-4GpV-4J-F%4E0.$^.ZlL9ZReM2eA;m7d,g))4d;%DN#<-8GpV-%t/F%8Q0.$l-/nL=sweM4wxrm;2D)+)4d;%BN#<-<GpV-#n/F%"
        "<^0.$jwrmLA5FfM8E:5o?V[A,)4d;%JN#<-@GpV-$q/F%@j0.$Q;BkLEMkfMtU[5JC%tY-)4d;%VM#<-DGpV-*-0F%Dv0.$pESnLIf9gM9NUPoGI5s.)4d;%IN#<-HGpV-xj/F%H,1.$"
        "O/0kLM(_gMsL@pIKnL50)4d;%TM#<-LGpV-('0F%L81.$o?JnLd%ukL63YSnSHeM1)4d;%qM#<-PGpV-oN/F%PD1.$SGTkL)AT;-sW^C-iI,hLb^rGDTjA,3)4d;%tM#<-UGpV-Bu-F%"
        "^;r-$bK>)M_w)iMH&uD<X8YD4)4d;%3N#<-YGpV-'$0F%Y`1.$;c*rLpn6mLcg7dDaiq]5)4d;%uM#<-^GpV-NC.F%^l1.$`?,)McQsiMI/:a<a+3v6)4d;%4N#<-bGpV-/<0F%bx1.$"
        ":]wqLgjAjM&OP,M=WNmL*>pV-,30F%e+2.$xvFoLj&^jMe#oDEJ<CkF&4d;%tfG<-iGpV-nK/F%i72.$Xf,lLn>,kMlc,ZGl9_M:)4d;%7N#<-mGpV-]n.F%mC2.$6P&:)s'F_&&4d;%"
        "3#T,Mw>pV-C:E_&pL2.$'?uoLuilkM0M<^4bciQj%+Hv$&iG<-tGpV-QQ4F%qFm-$Et%##f2,F%9$q-$tc7+M)8MlM?/NJq#Zk]>(+Hv$iO#<-$HpV-RT4F%Ron-$)J+gLN:ArLS3AW$"
        "+5-v?(+Hv$sO#<-(HpV-U^4F%(x2.$oD`*M-i@mMCSfcr+MD8A,c.TAx`a`s-`%pA(+Hv$wO#<-*6T;-?Y`=-gO#<-0HpV-PN4F%-(n-$4dx^]%',F%2@3.$voI+M44JqL1:SqL3xSfL"
        "9[XnMJ=$#u7e5,E)4d;%,M#<-8HpV-h8,F%8R3.$#&JfL:X+rL9kFrL*kqsLA6LoM&OP,M$$YrL*>pV-`w.F%?h3.$#'PoLDHhoM&OP,Mq5urL)5T;-$4ji-LQG_&%.Z;%gVEs.U2^gL"
        "S4Js?E8^VI)4d;%`M#<-FHpV-M@.F%F'4.$eXDmLKsQpM1VW#5I]uoJ)4d;%)N#<-JHpV-mH/F%J34.$bF)mLO5wpM@3Ej9M+72L(+Hv$#O#<-NZlS.]Zqr$qN#<-VHpV-g:2F%VW4.$"
        "*UI#M[(9rMTA+p[YB(&P(+Hv$(O#<-ZHpV-c.2F%Zd4.$&=%#M`@^rMPsiVZ^g?>Q(+Hv$,O#<-Z6T;-pT]F-8O#<-bHpV-`%2F%_go-$'::R*-A2F%j2p-$fHi;%;xS,M%la^-B<K_&"
        "%p5.$We8(M*W+vM0RaYl*E28])4d;%BM#<-.IpV-*,-F%1>6.$GT<jL7PLwMq9Vs-VS4_J&4d;%IfG<-<IpV-.8-F%:Pq-$g.b9MTb3F%@l6.$Ifd&MEOwxM,.IAkC'0;e(+Hv$<O#<-"
        "FIpV-0C3F%F(7.$;g9%MHX[&MGkw&M)sU%MP<0$Np5`SeN5[ihK2w.i:K2R<U'K_&W-K_&iZ?s.f3^gLLOPGiX:l%lU71AllZ_w0_BK_&,*+##0>-F%]k7.$IaNjL`8-)M`PQ)MIQrhL"
        "k.s&NiKK>civt1qh/q.rN(A&,mD6JrlS2Gs4i7seYC03(4TTj(3O&Q&BrW>-'x,Z$0M#<-N5T;-jX`=-1M#<-W5T;-p1^gLSZKW76rj-$<n?iLhpJjM#:#gLt&^pA6rj-$>$RiL37xmM"
        "k_,Z,27XMC/4tiC#N,W-A-G_&<0G_&P>###;E0.$B<wiL=kFrLg9)/#5eA(MrWVtMEe=)Wa-EVn^*arn35l]>D`fQa553*MdE9vLo%<*Mh+E*MZV*uLDJ7m--EL_&3>(##1q/.$aD/vL"
        "(@,gL%F5gLEPL2#gjWiMn)MSe^f6#6*16?$u,=G2*5T;-6/]^.8fGxkxXlS.-c68%KO#<-j6T;-tgG<-OO#<-$7T;-2-D9.B9pLp.jGPp>1^gLIoS+MBg1p.t^%##a$n-$jCp>$cO#<-"
        "$o1p.X`B>#e_(.$2M@Vm#JmK'-w3m'$,ire:*2)F80i`FLacof%nTYZ&i''#mLC>#Y8'.$VnVV$t;G##f5VK$O3.8#-Mc##*^Q(#CoCv/.Vl>#'/[s$:7@W$fBQc%PYl##?MUS%8R<T%"
        "B8YV$5=RW$D]qo%9O3p%MVP?#+G(v#&dl8.1d.-#COHa3Xk^D3$8Iw-8(_+*cpGn&<(%<$^Pr0($QCl04n@X-6ctM(bkh8.+aS+4N29f3?;Rv$>?T01L/'J3AS'f)G%`v#h.TgL2/[W%"
        "XiR/(c.a`W?)exX2:rw,Tqu0;E+Tl'F*7Y6Xd-;B,%*_IZGvo/+N?T.goWhg?@dJt7`H)*l;s$YN;:(69i*t$:<';.p*BF6e,ZZu?8Cw-q@(kL'_ZY#LI$##kFO2B+nR9&UH^#.4P(Z#"
        "&Cv?$(MC;$8p]Y#-5_nLaF<>5[c<hLe5>X$t+rB#l^'B#R<3jLxjqiBqkA+ZpV*V#7-J+V%/OV#(snh$v5DO##bA(MN8Q:v4c5<%`d''#/Ir9.J;wk$AhHv$/w%Y-261S*FaJcNIS(%M"
        "6I(DNi;pfhJ%CJ`2Kk_AS$mR*O#8S*5TpsNSI3YN:d$A44EbR*fAcJ_j'C`N_Yb5Akas%cdno+sSw7AcGNR(#dHm;#A2wk$>)V$#[u_Y7r%)v#4o>s69+%W$=Tl##1fl##;Q:v#1T]Y#"
        "UG7+#:AEPAnxjo7O>kM(.3k2(@E&s7g<KZ-*$SX-SJ/s6JUvY#-tntBmbGj#Q*3M0S2Cfu5vou>63](<DtH)8@,IBP1P(iuhW$JqK4V9`]$i;%0C4(0.####FdC7#OaYs-kG'%MKj?lL"
        "g@0%#ij9'#%3/O(,JY>#s5>##3iU;$u2dd./KA>#hFvH.78<PA`h9N0w$l_-drH[-sio#,rsUO'%CuM(A,m]#XeRs$MO2v#oPq;.%)TF4mbh,N]%hH4M&&#SsEGuP+d-&[mO._HM%h`A"
        "Dce&f7ISk47?PXo7F9`Abl9]Sa%L2B*xPXCQ3bq'WFDi#P)Wdu&hA:v((c^u;O]qLtM-##;u7T$<D0sLt,wX7Y;G>#LB4=$+AcY#i@xb/4jf>#YQ1j;DuG&$5Q:v#`bYF#`x@1(?`Y##"
        "dW^Y$orqB#40_:%1_uv-XiWI)$^.kXje6J*u;Tv-^?7f3(';SekQd:Bb`OFG21L4a/MCQ4q,JMR]o=*V,HnW$=),EGBG4Os,MX[FF.Yc6`Gtr62gZV6p*P:vma7<%`d''#+@]+/FdC7#"
        "M/uR*I[lR*6el+M5+P]unr$>NkEo8%]I.O40n^A,hEw(bU.eQsx7t$OpS27#5,LL2xhi4#(E7`$;m:$#Fw`V7:coG&65'p7,I.q/vN=p%(aMG)F)^m%dn@X->8+i(9=S_#Xl*F3DeN;$"
        "$5OWjSw?S#c+Lc_i*btJECWqWpQ8W:`^-qu<nb]$K,Miu(;2ZiiQKI_P]=:#KMl;-f)sJ5T?O&#$O_X%2YL;$,V1Z#<;ur$6j.-#H)x/3t48L(WBj>0M0fX-s+as%^ClkO&EsI3^n(Z$"
        "%'Y0P@<B_S2>*nMPiafQa_g]uUm,]M`#Q9NbpaiK6LU5MG0B7NIe@$TkM`6W#QH(#&,CK*ml2K(`JDG))Yqr$glR.9Lr6w^NMY##Fen6/vcf=#pj?iLp)3$#B:M0Mc_6##=2#?#;PaPA"
        "X4ofLbQmY#&boA5j%0L,[RNP&R_[N(`cS^+9.U-M[l6#5++AP#%DuY#w+v7$sAaig6f:iu,SO2B/mFSMng.H)jc-d*4_Zv$Og4FOWSfcrRk``brNVIM`RU[-^f2S*nXN:2R,8S*Te4wN"
        "C%g'1YZIv$2n`=-O)&Y-:$mR*iW#;2EQrR*S#jR*+2&A$+8*3NmNx8%:5ZtL_iH>#<G:;$u&A-#@^fW7x5R>%Q]C;$lb8&vQXmY#/CfnLhnv##kgn0#oV8B+)j7a3*^d;%Yt5#,aUE2$"
        ";EaigviXLu42Y[$PC?>#iuQ4o.lM8I/1CP8f;iQ&CdQD*GAA#,*P:v#uAP##t[5m8G+o5Bd]nr6ftkU%YN9'%d]*87sj.##47?c*j/d3'QGSY,RNXp%6LDs-%3<l(JjE.3(@n5/u9Ss$"
        "883F3jZAX--$ct$?2EorYvOjWFC5C]Trf2Xgx?r_3Q)guQpHlfm5m@#J(Q?]9:(N?FP?+HYajpu0GEO8Wi,Mub_C*#S1b(NEbfcro'28.c@cv$UgalL>q%Y-<*mR*t;5/NtkN=cxjsR*"
        "`pTGOtx'&tK._f`C6.(#whi4#5=RW$:g1$#Yim&#cPwJ1<HcY#[(0%%,8G>#7a3WS5LuM(4o0N(j^hA4rS[S@vDbiZ2b+m#'Zvc$?@Un#1pOwLgM6##+G:;$>JGA7MXI%#8Njr&;xqV$"
        "4;5##D+l+D/SYY#BF(,D8+Rs$#g?N0PuJ+*`a(,)$3nEN`5:q%.jic)V`oF-ElL5/9,B+4emwiL.[ZV-*_;;Zi0OCsoRb-[1L8PuvVc>u#9vpM2s@nNuo*(NU7pr-$B(%NS9ho-qjrjt"
        "S=x<(j1<&b(%:'#Hn^4f_1DYlNBn609cd>#2GjPA4r-W$`h>^##Z$<Ae2d&'0w3-v^IOe$u'FYl$`.)3@e/)3WvQ2BAIcpO44?eO*)FB#*6ZIq.<qE@3rHP/It]@#S(O_A1*7DA)WCh)"
        "$m_l0Hu.&4@BrHMSgvO(X$vM(G(r;$'vrEee<K`*rIGSd;L+@5&9o:d/(ckk[8BetZXUJ$`B;`*FkrJI5)P:vKGk;%m?qS-51N/'b1Pv$V,K1:X'GR*FaJcNIS(%MI@0_-JYsR*%)iKN"
        "SMU[-nntR*Nv7S*DT8cN4?KS%Z@gl^T*S]cS6v]uCX`o$Eu%Y-fs3bI<dNv$W,K1:@$Wm/a;g.vLTVo$A;r$#s?Ut%/%eS%';cY#tAlY#5df>#Y6Pi;%,,##n%0$.3=fnL)H/m(mgNU."
        "pJs&+uA3I)#0Tv-XkC+*I5^+487X%$TIBX^uJJu;4e1&Q,qgb$@FXe>,2o:KY6d2_V2;YPud>kFS%I8%kQ[I-LeG@.UD1(C(x0'#MlE9#?JAvL*W?##4kMV7LQ;qMEXcb#_&vx4&GD6j"
        "Bk+)#Qa@L(ZP8B+NI1N(H0CD3IW'B#^HaG&`%&Su%P`Mi[^5iK/%c@O7i###K?)MgdW8R<fm=&5Aj3/`Qcl8.4_oi'_-*g26cKZ$IUe),rX4kkV24*$S9XG%g`UV$Rk<p%C[uM(:^d;%"
        "9'5m#hAK2B$'[WMI*I9MIg.H)WNt]5dJ+<%T%x(*FP'.?RoSR*a4n8%m)7>#5IM>#7dHo[.iIf$,o'hL:GUpMqqrr$Rm8mL18Q:vgZB5B=_LB#5UfP#amdmM58X1B^^vA#o]lEIV68w^"
        "8.&/1J-up%9lCv#/`1?#Yxw?Ro5E$#5?[eM8m3Q/sA72.=(>,MRtQZG+$vP##3p@F[ei[#ih)GH:=<%bYPWtuk-6l.$),##k;8F-UAHX0P&>X7s-=9%W%ffLc%*v#'2,##Y^(Z#Y6Pi;"
        "Q$xf$u+f)<]5RL(gWbt$s(rB#5?#g1-nM8.Xv9u$dUg/)+I==$j.S%10GP>#b$;u#P:6p.=QvvkZS)KtJq_,B`@_&uNV+g/B*tFuad`,umh]D9E(hN(S<B^-025##tMp8%SeDv#_&2>5"
        "rd%##EEGJ(f^tX$<G-[-YGrc)Fl*F34D.&4fLP$$T$=A#rZ-boF<x*%4ROn,d+3Iu)[DgtM5DTjN,WStuP(e&.o?5/L3#=(iuZDa>t`=-Jt%Y-,qbR*e8<P$qIajNC/G$M4DNJ%Kj*)s"
        "T'AAc73+V.vbikuRpmt.$#nx4:>S-di@D;$>4)v#CJ;mheZ8W$_&2>5#a_;$5dxu#?9-,M?G5&#bS4jL1^Xp%$Fqk.-<Tv-U^Ac$=^u_PXWd<MGLs888+$rJK=uW.GO^b+]MW7S]L&c$"
        "@&R4oLqN8IMDW]+@]Y)%FlLQ8ZM5W.1]@j:EvM*Iv?7f3&jE.3cjl/(ml:^#483F3jap]=t]U4YW#ke$F*UdS`Es>Uk)jj#ecF%bustr6dM758LQ*`adN@@#v@BqugkUP/'EO&##)>>#"
        "W$7<%`d''#a=;u&;hHv$.n`=-s=ix$<qd;%%eN49k%GR*Te4wN+f^@%ih&a+?Qqd4<b`=-Zu%Y-@h*[Kb'Mv$Fv%Y-/_4R*uxefLnI#W.rfxM9Z45;-@9Wf:]*4Z#KoKS.2ZL>$Gd:O'"
        ",S:v##Mem/7@Ok'uaC_$eY:J;S(Vv>6J&2Bef387N)Ac*q]us.?<o=-Rj?D*J3Op%Bp()3:>`:%u7XI)q;Tv-Zk_a4`?7f3O/>u-a@0+*cjBB#rf.)*G9T[#=LkY7Nx=&JdvCc8+bXv:"
        "LKXs65t_]6)frB#&L-igv0YcS:xm^kCh:cu4MDpu(w:0h9OLG#mZ`T#YcZiuU#+Bai_/d)NAwS#OaIOo<3/s7aY`Qj7IUhLsY1x7%#n+siC2N965E`#abW4oxHS8I`[LS.7k4[[wkj]7"
        "VvdhE_jbT%00:x40M5##cU,x%41Is$=v$s$b`P)<QQ/W*&MZD4]pa8&UPOKMXJW+&Us.-#T7Lh;<_39%=8w8%(Yse+WBD15XIET%A]ek4A%WWQ/xgY?K6XD+^G4;-C3/E++pIL([^#R&"
        "CFIW$,85##a#Do&lL<j0-rP,*1C6g)WCn8%JO,<.wYIh(er0P%71t0(n^Fx:a.Pr)aq7V8^SH_u%b._6AP[4AVrv]uJRaKJ_&jYK4(P8IDqqAmTkt`pe*:3'DISV?#w3iukCQ&MF46Y#"
        "4AgkLMbm##3'E`#a2+##=jM2BDa$)*Q0'B-'/###mPiR:l6I>#x'l_-H*x9.xfUl0DeRs$H0x9.6.X@$pJ6E&44=/OfI?/['qVX#8-r@Rji2+Bxu[f_4'KcuYn&?fuO%k48wD1Mj/%/i"
        ";T4#?_G18'o>U`3?c$##4l$W$9Gx-#+;cY#Aup'%A6a87c=a`#9PM$#S82O'a0J%#NahK(%_bp%(jic)L;/d)x^av56l*F3LA^+4W=p+MO/qc)O6ANc4j7RjCCeP='eZ#o@ItKd/Tqr$"
        "fnsLuM:q'JS3_Y#$g)]XE]Q8IFqCx-`/;W/*Auu#(j^s$o0g*#m@gl8UKrP0r^nq$'/B+*r`Q_#c<:V%S'0hQ4*XE0@`/sUq-Dflt&QrG%7-/[a'U.[K^NVjN];,,W_Q0E]0%Q#?_K)&"
        ",M:8.?fI1pw($<-p0SF/HI:;$2khpLfDB%#eF8$$+Ml>#uAP##*Jl>#G5NI+>_QM9n>KZ-528L(,SF_$Ui(?#..R,X2D;=.hND.3S>=FbrlQ&gjID0[2WSY#t9,<VCv1pWb,QTa8;Mk#"
        "'%Uj$7GR[Mfk^oIux]GM8t.H)LJHY>2%ddMId3bNJY1%MgC?uuB5@SuNJ+sNk5ZQuJawC#eiQ4o*`M8Ixgel/q^o],7Yc>#Cojf/7P/2Bffwr6lg+t-6Sv596/?v$ODDN0N29f3pwkj1"
        "1a-i;Z0r;?\?;Rv$#VB+*_XM;$M(*`Vpe65DMF=0>tn]LMWJ';FsR[-QS.Oq::=d4W@#1REu=$2KkME_@)7DK<f0C>8m=Op/GrY7Dh%rO;w0>5Cef'8:fPY5VH`C]MpRWU-qTWU-2P5W-"
        "qSd-dq_cQsNbDDbBn%Y-S2,kk:WXR*<EwwOnVKcNN+[XN6lN=cfe(/`Ul`s-cWn7RN:TDN']15/-H1[[ukj]7p>CY$2fLv#:lcY#dCSfLZEbe+H$eZ$#rL_?VbUgLFa>&#0`L;$S-[0#"
        "@ZJbc:;3j;(>P>#6BG>#^S#)<HU:a'B]VA5S>P2(gsdq;[-`KH-IfG*B-kVQ@,/W$tII_968T9V*ng_0^/vV7UuOsPrGVPMT@q2)j)Tv-;@?c*bapQ&qH75/D6.l`dpS@##DXI)7AeT%"
        "uU^:/Qw75/?;Rv$(o0N(*C729e0Xeu?MD>F*U>[II_'d<A=RYhW3Uq:5]<hP6:E,MdoBdu,MKI]%hDK+xplK(KWPPO4@duP9tjbR%D9Jhrf*##uZP+#6Dba$6Nc##QiM>$*8G>#.<#GY"
        "=wVf:Is`<%L87L([Js&+K(J'J#G3]-0NFiZ2i7j'W2irtiB;/Sh09k2L,GX7T5,##9rhV$@:C,DW=I4==^i>5xd%##A-gi''BZh#$&5?,cr2]-:HToL*0fX-#9))taBc'AedIH#&obqZ"
        "k=hL#,euiuepb197[)##]IC_]c[I[#-ZhpLJCo8%0va7%cN<S.h9sR*n)-'-D^6FuV#a=-Uw(*%nnS4opprjt0qRktL5_nL<d7J&o^(,)YA9gMJwnf$`p*P(t2_e;p0(<-0mZ@fvl0vE"
        "r$FRNpe`,uWH=3IRYn.$dcISURve0$=J22%*a.H)k<#jCNqHHMgTfcrw7`Ka<h%Y-<*mR*c9rGM8;pfhw7`KaTG:@-'3gL:1-PR*Y7pR*c9rGMox#j^T*S]cXxd5/B3&A$1JajN7ohge"
        "G'MRaADoR*jKK:26t7T$s%X$O.kEr$/7Xc2QcriK:)ur$52Tm(LGq<.Tk@>#Sw6Z5tYCl0(ddC#'m)`#Oh2D[_M6mu1xBP$2qOwLx:-##`1RA-=X/<6H&>X7e:iv#*>Y>#uGuY#(;>##"
        "Y^(Z#W3Pi;xHH]%a_jl&P&Le;KqT#$#ONh#I1,.)@vxA$w?h(VtH;1[%TmaU*S)Kt'kY^tx]Dgt.q8]tNb_tMbexb-+0gQa9Z.WSAb`=-w?7r7E&GR*Te4wNW%iE%<qd;%Sk`=-(_Me."
        "$),##%BrP-gc(a19'jV7XLIW$+;5##]x:q`ohGL<U1?k([3x<$/wZ0)0pRfLxc><.IW'B#($mf1eo)nur$o2dqKg@X@&Cj$u*8t#b9*LuU]FIuoS/(M)cDE-TaDE-YTtv/3kMV7FG>##"
        "(apo%rHJ%%9I_P8iAY'ADWAS[n_@eaYq2>G@9`?u'Xew#m_(c#++P:vjKT_-/T(kb^$MW&,ecgL>k`=-Vt%Y-8K(S*Te4wNv=Xh%Bqd;%Sk`=-T)vhLV'buL=:xiLY5w##gqts%-`U;$"
        "wWj%=UTvu8K7$##A(4</=k,1%WDe;%FIZV-:eS@#Z*#$*+g/Gi4(vf:7bCJ$mOq(H4jGPMS+epMccPw$FpkM(uYTR*]UHgM*j%Y-nntR*+a3?\?XU5s.tM(9.a1@8#M=.<-iPC+<elWYd"
        "k#Q5&Jm+@RX_JG9Pdw,MdpJc$>FXIO)-LhM^Y9>E?$i'#1,4*vZS+q$FYI%#nlSZ$bu(?#0i1?#s3IG;g$0%',Z_$'@cgh,#Gvu#*Juu#;m]Y#<]BB%JDsA+.=Lq9`30?-#J>V7s/Gv-"
        "#3<fMm=?>#Cjkv-N-*hLF6K%#nV7L(1Y<W%s.rB#C=>s?AWc8/9-CHOVp9^#en742S:Pn0.4J+[>32#,ch&j$s_B:<Zu&RJ)[76Oo]2ShusP%.GFq9N*a8%#bp@Q-X@V&M1n$&O3Gl=$"
        "'-##>ho)Idx14GMCr<V7]<AY-$W_4bCJ:kFpv_c)<vs;%CYioaFt@X-s5ih.cJLlu:6r-/m$s@$R@Pt:lE[Y#AUS4op;_D3lH*20D8Qs8BM1v#?>cY#SiL;$,2,##&tCC$n0g*#-fh;$"
        ";Jx-#6S82Bk4'p7&aC;$OWC=%nJQ8/]PqZ-dV6W.wY8B+H(1+*8v19//IuSh[2aI3vD;T%asxF4_R(f)oF>c4*;Rv$acu+`<])q80xW%Z3FLmuVHo*IcGk71A<g*c/c31<'oa,[jkFa5"
        "J.$39[eg<]bG_M?+6u@,Hro1_B-%S<GjmTIS)8KED]*//66A5/@f%=(X9R8[,w`=-Mk`=-F#a=-dk`=-C)&Y-Z7tR*Vwx:2^BmR*X'#;2V/sR*DKiR*jQT>$FllhMx^A8%THp=cl7vo%"
        ";]f._HAf84N)P:v[5,W-]hB.6F[.SE?=lR*?0DR*]tvu#WFW4orCeD3/,V`3+<1YcEe1e<K^(r.Ue`i0T(Is$/l7w#4(`?##F[G;bQ]:.-uL_?i<Qg1SAP##=Ia5&@Zu##0i:?#L8MZ#"
        ";Ms1B8S5##hqcX&+P1v#7Q(v#b`5)<Q[cOUNRC`Nk^6+#Nk'l4G]r]7O%8C#(1mc4?-rk19_Y70TQ]A,&,]L(JRes$V;-C#(Kw)*>L)l(u_:W-'r268['nAOi1-J*t4:W->.;8.uiWI)"
        "*)TF4TA%$$;FlL$+FD*ET_Am23R>L>Bg$>7=uU4X6twYAK/A#8ax9c7MY%#-f8@&@Ywv,$OUH(>bc'Y8]8+r&g]H_u`REV/$n@t._c2;ANd*>]b>1m>*<c%,Eb/TAY9DgOwduhZ_1J>B"
        "V.O^@x,'##eu&584Imv$0e''#nJr9.FG+^(<jU/10v7T$+V9:#Q'+&#Jn#X$G:2D+-cU;$5G(v#=Ut0(5+Ne+uR;=-Y?en8M$mG*o06&%4;k_$+GY>#S$jq0PRId+>Cc&#<=i.N;/[i$"
        "R1?T'w[T+*-I;@,.DRv$r%rB#6*HT%a<Tv-0P0N(+tRF4XZTv-Wfj**:)7J5SXS+[lt8tN8/49R#lJhP%*b[0b?SSRCd=RsF>rtP5-E>cB1:deS6.tDB'IR7TEkC21wk2vt67<$pe[%#"
        "5dxO(_<=k(+Duu#),,##01sdm*ahpLt-Nj;(&###Y+G4%l9_c)]E'U%Cnw@#a/w)*1TSF4Tgo;-1d@^$Ee0T%9M0+*Nu2.OcA9HbqMYRH4B'hdtr_RN2YdGJ)MS+65Wi?Xt(LDJ;p7,g"
        "nZ:kOu1b(NX+b208A1PS?1qv$xd]rp(LL`$qPf84YN<)N_D0cNXCxJR4l:S.#WHF.T1n8%k^0`Ae.or[md;#vu?RW$%vDE-U_6T/XLB`NMx)8n]NqD3<;9L(D,j]+HfU;$vQGF#r'if:"
        "^=Ds-:X<D<hhfs7#a_;$l8#nqQp<V7X*s=-<rZX$`^Np7xCV9/h$Op%*&JD*m';<J&n`^%AFSv$XKD.3s2u6/4C<)YFw*8TcSKm>r=dE[;kJhP%_*[0Sg+)jOeacOHaORsDD7:QK.YGg"
        "-WQ$8fQKp9%5YY#jGUEnE'1Q&vAYc2rF0N2FJuY#]Fd>-IkNT%61wo%FELfLn^Z>#<&#v#5l`:%3=#70<cb.)W,w)*Wuw:/iVp.*hM8n$Jop*>lxe#%A9*<%P[NT/x'DgL8(AH#jK)%H"
        "&@rxR$6CwJvmTVh(1BgJ4berCIKCcN+_Sl]R]JLVD`76<t*AlXM&br*Al/:)avmJ?\?lf7IO)gUsDEq'VY22<:MA/d9-]nQ.%Mqe=4#(^]a]W:vopq^f[[.)j<x)JUGk?8%A9oV$9AUS%"
        "3j6S[`-%^$*%v=Ps&-)Onac++m.VoI3(A@#t5Tv-;F*9%M#Ac*tx;p^&MIv$*Jl>#)8>>#_0k<:_#A-#epLj;5(MZ#q7R6%qEYv.Un:^#`Od$0$+gvfQ+s]753x=-FESa+<q&7/#Nr0("
        ")shl0:#G:.gv4X-RK4L#Hfko7r?YpoXcu>(.5AJOI.]n$U(JP#6([(E7CHr?uWjTJ1jAktLvT1&n&<LY`3L28(HLMU*^:h)s@]`*-oFS7,@w^]b`(?#s3IG;/`u>#.]Lv#AdC;$Sx'5%"
        "iUpDl3td(+]$%&,?6]x,u?XH*CS3W%r%rB#23=68>^Tj(Ynn8%Pxv58Bo29/,4mP]cM6[93Pn'C#W2JG-@>lm+Qh7@CPeZ#09KM#1F4Y2>&;;8?9OXVA'G1pq2RO(Pe;.[#*)##R(4GM"
        "4q/Z5BFAcVXw_v$rJehL>k`=-Mk`=-F#a=-dk`=-OAa=-C)&Y-sfU:2Vwx:2BFlR*?K@S*^BmR*qUZx9V/sR*@3DR*a$[Y#J/)A4tG_D3%E#,2JZ@T'/SUV$.8>##:G(v#oZ0[[(lj]7"
        "[@<T%0r68%DD>?#t+g&?K1$##rm5g1s2G>#3Z]Y#^S,)<-CIdNvI?>#(AcY#'Y(l1dks]77DD'#P'g60.VLkLi,ws.SrHw-=-Wr7awV<%]]WI)+2pb48[51%Ol`^#V1?p7+csw?7ns$/"
        "eE^r8_6PV/_(T(h*oL>*@RgM1]b,N1FW5+%:[;oUZ(NM08=2F(kvtF`&35#>p0GD*&+(<&(,,##$)SBfG[0[[*lj]7GMcY#0]U;$h1IqL^$u1B+&###Y0=;$^S>)<qrJ.MXRdY#n*5LO"
        "t4k(4(nT@#Yc@L(^pY3'LtWT%Abe@#0hj33&ute)?Y6^nLvnf3uun]4x#pG49dr+kEfuZur?pwtmY-)jE.B6*D?#_M$?&j9lHjdPs/5##W[eD$`-'2#Ic/*#8lZgL0lc,2;+wo%%U8N-"
        "N;AZ2cc68%:+RS%Xs?s$7%%W$e6#)%mPG?#F(^2BsXKp7dv)^#FiZs-6&@8@SN7a+lcXj5FChv$AFL,D^wM15FYr]7?nNY7nq[a5<la=.b$BY.x4PZ,m<Bu$x$(f)-<Tv-'ISs$@]WF3"
        "w,jhL/4pfLfmv)4AVB+*I5^+4#W4jLiJ8f3VVs/%b9C]$;,@T%Qxk@$T[GWSm+$B`.E4d*9#91aBjOdu$xoRbi/BRKrqNqNPsN=Lsrv3aC*aII$22'J[tO(IiDWt))>+BUioLK(&($-3"
        "3c-rT^f3SE/js#?4eE[I32.=AXIXFAP.eTOZ@6uL5XFuul$q`$mqn[#K1A5#01n8%TJo*%dZ.m0?mmr[@PhY?\?k_B#;R4M$RS>ca:2^c$Xj'2:9mj/M)IS>>PZVF7_)/F7V),##E@uu#"
        "=:b9#IQSW7[ocY#na2K;9/5##5Dj:%Max_%E/>:.gK=p%%EQJ(JjE.3-[w9.]77<.XCn8%/W$9.4TC8^R>g1Y-&ofECFPi$W_oT=Wv>Mu,dmkA+&uU0@+a]b<W%B#@X`kFn>#&#?8(2)"
        "-P1v#PA4D+(<I[%7%7W$+`$s$8vxu#3p@-#_gLj;d0&b%*vsx+KZHtLn3kp%,jic)1v>@09,B+45SE]b@'wLP8$s3Y4/>huMIv`/A$s*R/oU:v=MZ5/)cC5/W'PfLT.>&b:DUS%2g6S["
        "CRHo0mWlIq.%[8%>ONP&LnLcDsog;-newb%1a7+#=]sPA8+Rs$_R8j-shiQa*N5)<P:];%oj>%.$B&@S`Vr]7UCV?#P2)o&6'+HX[BAW$:XWP&nG$i$E_Ep.+^k1)d4p)vY2_^#:kPW8"
        "opus8[]b&uSnAau=0dm8h#D99vK:auwCWnMDSHeM^F:_]h3+&#O,4b$^JaPA1c_;$1e$,M1Jw/=%<`Hc]U3]-7.=`#[c;a-s084=E)P:v'5pv$0e''#dHE>.8tX2$K;lR*MhlR*Fa@S*"
        "dTmR*BQ7S*es$#O>/$5PZ9IaN?5-5P^1?dN7?A>Mu9wu#1b4V?2Cs5&Ej?D*':0JLk@c]#S&]2V%J%$$&,,##FVE5&3b'W7L+rjtn=vv&r4fv5(m_V$=;5d$bUse+.6t*.N;G>#A4UcD"
        "w&h]?bL6c*=M,##X(sL1#lS60ixms.9DJL(k1bE*;%i;-1=Ca'1r%r8Pc:*4](KL(JCn8%(PV='JZu14orpF4IwRM9U/YD4v-<kUVHw73[kefu?lfUmT^S2iW.$M&YaRce/kg'/5>d+$"
        "tNC&NDGULLilYW-UD2?.Dv9-83bk]uj:[s$6'Xs-+:rxLkEo8%T3%Wf1'.aNZgo#Msk$##9HCnuTCs)%8f1$#BfjX]#)to7`rcG*a^*?5^t[Y#7Aba$fS.v5$J>V7=5,##'8>##0x4S*"
        "P_r2)<M,##nC9'+SXxv$[k'</$GJL(-D0p7WNI+4*#dr8nhj:8-['ku0(j1U:Xd+;&AE0148)9QLKxIDd_x@tDUsq-USa-Zp3+&#8kMV7C8,##-F0o/1WS>#a4<)#'Ywm/Xfx^+rM1l0"
        "9f]b-53;%BdqI%u'Hnxuw]@Y/)?Bj0chR%bhV+<%dT-DN.I-_#MN*##$N+)jjl8E>]KJW$(8P>#=v&tqBt[>#ZEli;YH,N08C<p%UPRL(vg$iLHS1N()?3jL[xCpJ5@xdu:`;'kE62Mg"
        "$HoO/E:8V?Y=;(aDkr@k#)P:vf2U=cU3mEINn@kFQO>#Y/cl8.DC+VdOF=/DYl+Z$vvRn0;id>#'2G>#&jU;$qX%v4t/,##U;p(</5###A_Np%7fUV$@*kt$@T.FIM*x9.hJ-_u;+&Ck"
        "wu@ng?<5M$a<OAurtvC#odf(MUi-##3lr8$3/%8#pHt[75>,##-F*Q8RfG,*3d/qq,/x(>=hA<7#J>V7?P(v#h9Z#$?mfn(>;r]79I7q.X5n-)aujV91&;)&Hp.c=P+u]uRc(rtsKW^%"
        "ZllhMS1b(Nw.kYlh?0Yc+i?8%?uU+`R>/FlI*_xXo3f^$S=bihXO1j_UgZiBpjkG;fxWw95ag@Mrrt;..O>#YY$.gLclU`j]lE.6WHgP&n0P_A-/n6M9&co77ZO++o-;Q(vueh(@SP>#"
        "74E5&'8YY#1xIm/<AlY#3^xu#9R0P'CSbo7/wXQ^@Ads.v`Ll0B>m>>7<j<%_ZAX-8_fw#vxLX>f5%bOOIJ(a;hCMg`3`x4B[FT%K@VG>UWxo7HB;4#$),##7H:;$PdH(#<TZ^7)Fq+*"
        "7GcY#k[Me.wkj]7R,IB48VTB%4M>##b:k_$*8G>#rFse+o@?M9wP-L,/1Q]Oexr]7Z>ou,67?c*pS`k'K[ns$LT'B##==,%4a9<-x5BS%^D>H3//'J3HNKe$-1kiK7-35UW=$:A==h@5"
        "xb'nTHd`HE2;=1+GObD3Dc>/upr`-CLRP)uTZn20Q-Ld6@EQ%7o5x^fg-*##cDw^fTqRc;T#9b,N'^Z$]xuu#A:NY$.)k_$-GlY#UTvddgOJn3k147:ROC4+A($9'U7Eh)p<q/)gwNp%"
        "WsnO(N>,q^/4T0%QT(5Q8Vlh@cHYr*lcE,FaA^>,XGWiWdE(Z#)dp*7'V*Bu=LS;%5Pj`#D;?&MQCo8%)h(T.%/5##%t')0Ywg:#UQk&#-2$]19MG>#ZaS[#%CIG;dk&@'0&R@'/-9W-"
        "9U)I-Wh@W-R>/r00jmi;7fL;$^S>)<Igks'$N7+#6-XKl&$t;-`DD3:;XV6Ndmo,.a>p`?EQDk'Wn)U.?ax],+SbI)Sn*9%@'TM'wG(l0>R(f)KIw8%u[39/+^k1)KE_U%:H'=7eSZ7C"
        "%Rj785m(q7j6ZNP,hbh<@o/,,t*:m1mFR^ZmfOb+U@6r-0E,>-(?nY%%.PfLLOQ:vZgT=cGtfD37w+&Y=<=r.fqWon%=lR*=wu6*J,>>#Gb4S@]/8(#tn*[7Z<S[#r-@G;/DP>#*9>d$"
        "RjxB/Tgb/:BK7a+%^u/*RL?(?*'PR*+cWK.$>*@$AoDt.(8-<--D2e$hYMK1jgQ/9DHe89fJ@+<wIK?pNIKA'q],WT17'd[W[7H-`luf#Pcp*7'Y3Bu(8EFMj<p7R.Ze>,h212'jZAJ1"
        "N[)gC0K/s7Eam)3l/,r'G9_c)mlIl'J`Y##0L%k9q@0j([D?p.p:gF4:Iw9K/;V9/D.QRmu4olk(epiRI''oNmqkI$'Z[WtLoJM#:Zg6t,Pp(h]_VcgDlCW8Umj'/66A5/?bs>[M7Q:v"
        "lKMv$Mk`=-Uge+%#N/m0?mmr[A_-Z$vm`=-V_:@-^k`=-@#a=-lu%Y-=cA.QEc`uN?mx#MbQF&#e^n7R?;f>,[G@D*mK*20D8Qs8+25##8a1#$:C.@#$h4r7`_r)+e:dl/[?8N)<H2N;"
        "#`E<%rYFo$o/6T%#407/c[NT/L^D.3>lmM3+Tm1>3jR'ef%GO:9mHFHu*8pNXRjqo=:U.30(6dNAa=u%'+>o7hh?qs3Bmh0dAR:Z+=tOONY3+[SDU7%lf/+[[1ns$B+P:vJ>Ov$=@qS-"
        "vZJs((qj-vF^1T%Dd8xt5F55f)XMS.5PR]4ig;a>U=ns$HilY#6o(v#uK,+#U6_,2EC.w#;Ms1B4M5##dLK@%l=?97'Ans.897f3J>pb4$4:K2c/#^,tc%1(DI[s$m0VS.-Gd8.eg'u$"
        "HFt*<MqP,*.n80'==Ds-ocYs6KI,,Mtp_>#2=hKCMF/23,+K0>w2I9/%CBASk`wGIfe9W.Y.#VB9:cWB(YIf_;6c?#6VlB$3/jGq/C,=?q/x;6fdJ,GAWB:Wxhe%AGHj'fM6G@`R8%Q$"
        "wduhZ%/5##H]eD$xcH(#Q'+&#Jil**N1V;$2PG>#,ast&Xr9l%tCv>#5Ds1B,>5##]c7+#.Hlu>%lea+,5ZY#3YZkMjXm>#_9']?H#fh(VCGc*-I;@,Nk39%FKPJ(RFVe$Z@r8.*m)F3"
        "2dlG*5,>s6I5^+4c6a3P#%su]mCl-YFT5MVd$b[0CXj3M7+N@?Na@:JFEkX$&<eC?R*RDuhFCBdrE_TNO$))jOPsx+._%E5.x:$^Z+`?#v<IG;K?+v-c=ugLudd>#3As1Bgp+VEwQdk("
        "v=oL(>Z,8/lAWa*1;&RJdAZv$hcXjLpl-AOU=0>G%nvtU'baVNnd()jZ/x5OY=O&F0OS=UMwH`#dM6V?EnfD3R`8>,.[?T'dhiv#W02LF:U629pk+ND6+lo7n^H_#3BlY##Jse++,?>#"
        "3A6:(V3t[?J24.)R%T+*/OD@,B.i;$HWlf(N/h5/aAoO(9i+gLc039/ND#G4nRSw>AZ?T.:BDaQJGk1Y0POcP*4sY]hD1FrR&a60u2eR9f$Q:6NAD(=ClG-0(:FSN@J=cD%L1-%$F;ig"
        "L46p.s:AD*&^mE#oJc9%=?LG)*M(Z#*dcY#-*Xp'8+uA#vq[9UtfG&#)/>>#JMp>#G2+k2_-w>#4S&QA::w8%xXse+9TvY-FbP&#1elZ-=)$G.Ge]QCP>1@-`Ib,%C]`]?xEb$#:XI%#"
        ";-<1)$W7L((shl0%<oO(;uiM:FIo/1KUNx-'6dJ(9(jvWboa#-vtA>1/WTCO3G='ED]w]+Z*9hP+mlIUS_iku[k<7%wK*##7o/Z5/FEul+ATv$vNf]J;b`=-dk`=-EA=r.e2=p.NdZR*"
        "SR>#YP.Jk4CXn7RQFAPo/9FM0tRKj28;P>#2wow#->bJ;1_;+PK/->>2bd$$Bvgo.*grk'[)hB#5DG4%1U^:/KlR_#92FxHACML+$1VL&Qc6,2ep6dEQ_Vh?GApp`'2S]QL1FO3aF/XT"
        "6ocSGw-usA'J/)@u[[E5j(taG8j1'9$S>*4puL[6.)7J;R/>iOaoacF@:@uu6E8ZMWV)&N?ZsN9k$U#$?-%kt>)NS]7+P]u0Lb(M2n%Y-EBx;2VhJ:2>][7PIq`=M+`?>##QU4oda*)j"
        "?>TrQmQT/1F(dY#=rG>#9=ae+lxP<-h4]T@.2h#$7Dta$VR*T.4G&2BJ(@T%e'WC?_U$AOkn`;$EG;t-Y%#gL/cvY#:P4.#WnW[?$U0@#]pLO1>YX.)jfmC#@&+_$5DXI)]]WI)Q5S#I"
        "gAE.3FH7l1#.@x6hUOe>La]1=u1j^?U*,aI@;)ACkA;r87*kNC,c8)I.w_T9f`j7:5bp[h#N[$LWP3t8o][q931jEHZ#Wv-0hBfD7nsQ;a;ZF5$`3Y7NRRI4^]97eXbKZjoo_+Vtt[2'"
        "`S@D*h`Rp0AMl>#2`iQ&J3]3MKFOPAXk+T%Ro-W$/H&##[Mp(<&dIL(hTOp%@fnv$?]d8/m_Y)4)T=P(ec)g;/r;Q/ZVnRTo_n#r,NW`E<c^+AZbEt0Ww&9]S)krL$PgXu5A>J?]RA%#"
        "pF+$-0k@a5$JFgL?<Nk'%-.@%/c1?#G$w%Fgn:S(U0oo%]pEF.(v7L(_@o-$&ntp%7/0oN`4fX-%m0g-N(_bRK?Vcue?HH.D3WQjB1YGjhIlDM69Q:vP(.T.>g''#f^=G/AEl6#E:G6<"
        "0ARmK4tV'#ts.F.op,?$0QjfMs'88%s]]uP4x4AuV#a=->)vhLbtf4vQprS$S@%%#:'U9%BP*wp.CX/1A](Z#XBli;%5YY#(RCd%LU,r)-9D>5@$.$$6Q:v#Vgnk;B+H0)kt0N(iWOp%"
        "dMB_%9mJ=@2Q$9.1[/>GsicrT?Rg]'e5^;T/&f@ub^GsBn5g]t;b_F>[<:Yu4;xxu;uuER]+ZlLD*6e:,:PR*MhlR*(85dMdUvdNeX7F%:_-Z$vm`=-kJKC-+sDE-^k`=-@/Au-&5fnN"
        "eX7F%.#6>dEMfl^S6v]uGnrS$X<=r.@?c6#&gq_AkPQ:288+`A:Av8%-OP?e`w-##KH:;$&4+&#5-sV7G8,##ua(($?iLE%x;p(<c>]v$7,vB=&c&E#&ddC#_um_uAo:cM_XQC#nh.`t"
        "%tI%u'J*1#u83jLIpNuLM:<$#ft09&CFhcDj)_>$juVN0kcRh(]]S^+Qf6gDL9VI3tgl#Od'HOZep#AVVSwV$N[]*@rl?,2Uk-2^g2K_A]+ZlLH*u48h;PR*dTmR*t.pGMH$58n2>a2M"
        "<h%Y-CBmR*mOMq;0IfQaJCjQarchQalG1<-M'G/%E9N[ITp`[&@M_;.lZM1)lwH>#q[V22jD3hu.Oo^s;<fP#V%X=lD2wIq$fO`NYHC4ftxV'Aq)6PJ`*B&46Onm'WT#?.dX&I?Tf1Z#"
        "pXEgEsgW$#bm'>$qxqB#;CI8%m1f]4:;.T%fpEuPPQ^pOsNeW:]5'VQ^F6C#kh.`t>i@jHjmSh:`:`>,$EGJU#)P:v?(qv$1nBB#G?$q&RXLv$/@7r7B%GR*es$#OThF<%;hHv$)@7r7"
        "FcPsebL@VmX0McDhB@#6'%%pD2penT-pb$uk$BEu+Ds7QA#4m&@q6U29(q-?4j]8&64`dMP^xvMUlH9%?0W8&7MUR*4EehL*L$l$PofA$lL7%#>m)_7P.)v#i(eV%1fU;$,YCv#pX<D<"
        "5&mG*mALh$G&###0W?e%cKx(<ruAiLL>Rf;u$/<-wLbc37`lE*01h+*M2Z8/jbn8%F#DD3<=1<&1TZR/b3YA=sB$j+$@fs-C;qKBxUZ?$eJv<EV*o)<`@&?*L)>>#Du7T$#,ZlLO)A-D"
        "P:PR*MhlR*F:2dMdUvdN-a;=%:_-Z$wv%Y-(19WJ:h%Y-CBmR*E1mGM)b>;dY+x62-pOhu1ofA$n9=r.AAcJ_Vh)72j@n]-]hB.6F:2dM?'.aN?p4?M&`X&#WQNhup6JT$#FX&#_4,a7"
        "]h`V$:j<h)>t]2'6P1v#*WG*,xIe;%.PP##)k?s][HSs$5dxu#jm<D%pPPA#GFYv.cjP&,CWdT/7m=;$@W6+#c+=Q/F65U.er?<.uR&:/Pe13(#aUl0*I1F*m@C8.d1NT/<ZRD*/;T+4"
        "HG-9.Q/`:%1&4*3fY$Z#82`B9n6Y43f;b*G.3^)8c_]*b:*mO32bS0DNf;B#w^E1G>Ot40kCWlogq?5/:Cl5;]cWUMdUvdN?@:@%9PhY?;R:B#;R4M$.cFO)OaW8&dlVYZ7>`5/Wj*##"
        "+aM,MR8=JM81sxLL:$##W.ofLo5ZM$C5>##?&$]#B)&#,.xRu-e<fnLYk?##ARR8%xDQJ([w(;ZtOBW$M2YY#H?uu#3,ZlLxF9C-K(l?-2X`=-`X`=--aw#N@)u;ND3DhLV'p-%m?WR*"
        "x.6W-.U5R*q'98nk%<>Zo:@/ZAXLAG3If>6[qN:2RXA:2'97R*x+A;dIP]?7j:'#,+H7:rv2sVZ'gWM_Q/NM-`i?x-Z/-aN^,lvM<#(]tY]#Akxw4R*tRIX1x-lfLK:?>#C6e?$EAP##"
        "I8$&#AE?X/JKq9&S`UV$PX[8%Y/rJ1ZR%Ak?CEP$#:;8$3_>kFb&a#$<cRfNP1`J@t1:kF)*#+%Mk`=-F/Au-*_udN=87I%9xK8SZ7tR*fj#TRQ9Q:v+>e;7[qN:21#+`A%5A;dEMfl^"
        "6%G]u>9q-vLrpgLQlX`WA:fR3$+W:;mq.W-xk5K3gK]8%Ke78%UH^#.Ox-<-sGeY%Z<x(<Q^[;%@nb5/p%rB#sNqq@5&:^uqAEP$+DWnM/[HgLd%[lLtW@$8[x4R*MhlR*0bQgLdUvdN"
        "pis;&V1@W$AB7r7m;[lb+C$gL^1?dN2js;&+#6>dC,&?7@hAw7fXLk+[:fR3&eo5/Zv7T$v`OgL0LJv7h(>)4cG]GM-c^>,dTx+2;Q[ftpt:U'VQA>#>Kx(<-nSW-V99F,jGCu$Y,3a3"
        "ZR%AkX5T_>&/>3LX4XluTsI%u#q#h%^#8rM#)P:v8%Gc-twTRNSf+RNM>-g)B2jo]hZ.m0?mmr[A_-Z$vm`=-pAA_PiuDE-vP:@-qDkB-Vm`=-Ox5l0qG8)EYC-5/#J5=#NxOM_8()P-"
        "=_M=-l0X_$v&0W-p:#?nM:?>#eoN&#=U5.2$rS:6C'W:vNM'##fJlYlc7TN:+i?8%7'XS%R;K-M)ph%kUqdF.n+OD*KP.R/v`U=cgGH>cEhl&#..s4#rCkxu=MB>@5TbA#Ob^Yo4Q()3"
        "B4IGWE4rU&IUe),CF[s$2o?8%r0@G;p5COb'u=.)pM:l0S,2K(073Q/Z$nO(mJCKsr9`TgeP1<FDh2`##FhUL6wF(W,9Jk4_k)##R*Fk=m88>,,8Z>#1Lit-f+IqL=j,X$kY)O05wH(a"
        "$rS:6s[=UM(tJfL.tk`<`/w^f*_do7cdB#$en_HMNJ)Y$$<;K*tkR%bNpVJCfqqw9Z7w8%gqG3#Qu*GMhSDu$</<K*9gDE-2@[./>9/W7SM:K*YIk//`Exw#J@Rd+P0H)4U4]V%47Rp7"
        "`9M8_OF-##I?uu#?JvD-h&c>APw%TAj=gl8*F1nB&7PFrUvG>cP.#(8&T#:;[$T(M0/(>A`xLkFqqdF.J*shLi1e8A)bO&#@wOw$uP^p0laLtL2gW$#mQmm'EX*9%-Pc##&C*=AEwi6j"
        "4p[L([D&B+nE^:%60fX-GMdl&2g+js[V^I_H,$a$nkv;-Cm*J-;%L[/<EAW7O]C;$*2t>$r6PThTgU]$X:no%,#9;-u7N9CIqLThu^B>c?@^qMi(U-v,F8W-.vI9rq)g&6)gq%4a]O-M"
        "xh4X$srXv-l>0X8++l&#1;<P$;LFF.T1n8%NeXZS0<*o$Cq)v7xU_wg;1ai0SpZn&C4VZ#.Suu#X1UrQOkOe)^KXX$o$E^6leDu7siq]P-LeT_++`=:g(IS#JQ1<FM+0wM@&=iAdlsC7"
        "HYx(s.rA^$utO;d0B]xFmeF5rYC+T@Gk<>dEn1X%mj#v$^S%2h2RHv$B$&Y-X+OR*N*+W-o2TeOU>:@-'_?a$?7/pLnn?>#.VS+r1QI#,4%rr$1:>>#,:@^2-Y(?#MRH>#3aiiKiJTU#"
        "J`uS.d5jI#NJ#W-3CF]'9eOw9p_ju>hqY,*nF<nho7A-);J>##,an`$6^EBOI3&,;/&6W->tP+ueIRf;)&###wTBp.XUse+%CVk+Xv@-#BV)g;Pq&TAa9MYPY;r]7P+g(1bJ*i(888L("
        "`c]#,taFt-x/M^>Q<Bg2IH)^>ZaJ8STVuCde0R:v/_f+V2`3;d>+YfC.'to%9Vf._a3d845dLtKp9+&bCW_c)*FDd4i/Vj02JY##5%7W$<HcY#4gGs-LQ14D_#w;%v<?JC$5Nk<$x*&b"
        "(^?Q/k2BwMGhg[MHr$RN_(8V?Wv2RNPNq%4R*0@#tve>#'D(v#=xgb$-3Z9M#ri*%Y>NPA*MuY#7siI(@4IKtvp[I-k4U4&*aE[BLl@5B]^0Y-lpw^]DP&q^nfhk&#x5LIqRsrRpk&9."
        "*T&##3^e>,6L_l8?ADZ-4o9f)Qq<9%;=[S%2`iQ&BIwS%,P6Z%W.-?#7/Qf;5fCv#,AG>#[9L@.LESA,/VFi(upw0##-m3'v7rB#DW@l$SbiAfk*/T%?II0WN`QOXaegms^Ns^Nlg1FR"
        "T4CVN(;D9WM;@Ld]cK8PJV,L$#:;8$[kL:QusNn%DZRLg6ojfL2H[Y#,J###d`')jjB*20c@3[[0lj]7L_[S%M'XP&NZv1%1+LdM.PrlMMl;'#Eb<T%@D`Z#;F*t$U*&x&9%7W$-cqr$"
        "fM>W-HiW<qwjq8008-=1icc?#P&l?-t-^31KE^x,grgo.W8:$-)60G.E[:N(0'=Q'v7rB#S?Km/8o0N(9,B+4(&^#I#ote)T8cdDUY4at=?pwthHJ1Fl%h>$l[mw$m2%8u@Aoh#nQ..c"
        "uR^3%e6nY#hbS79hOsq8%5YY#.d1V?+g+)jrsAJ1J]Mp9J.e8%L.IW$x/o>#r#=;$41.<$GPG?#bLu$$O[)%&0MuY#(D(v#2m?9^APUv-t%iT/nk+N1(*vY-[H5a+uk_c$qavGVThe*&"
        "UHZUPFG5g15D,gLQe@R&[J]B#hCXI3iAF.3NJ]B#l3O4o8r*vc-x;2LiW@N15Vl##2oLZ#@Z(v#(OTsLUU$##6po>#FEbA#E4#?.vM*EIlTj$#D?O&#,hn0#N&Qs.v%>Z,*L8X-;bxUL"
        "T]A:J<q(5gIAVT.Yn:rtZ4-]%(aQC#C9L(Rm%?FBwa;tUeCP?$fU9:#p(usMpiXU.ZhZp.)8>##E`alAO*$?$[ddAQdh2Z#)EgH+)2Q&#4Yu##Sq`[&7J>##0DP>#K#$5JVYvZ$J`O;-"
        "%J>V70doJMK[mY#bG.GM[;r]74S(5/MD[60fw/BJtUo-)+#%m0fTcohlo*Z%ZXI=-[xNN&]IpRT#e(6uMx*4''TwS##iY:Z6<;S?$Z`[GYlZ5PADkp'skJ44uK<D<+AxiB7p-,)9p1[["
        "1lj]7I6A3kc9)s^.lP<-.uP<-J<ZP0E`1Z#&8P>#iL<RMjqFPA(,,##$M>V7CM(Z#/jn`$@K+v-ZFOGM$:e>#sL;Q(CaLk'P$XT%?0(,)H90Q&PVG,D;.w5/41==B=4eP1JW*>LnQ%c$"
        "B]Ddu29i,@&Qd]u<HdR=-Y$##v46&M=D<Zmx#wu,&B^f1qAS#@D8>##EM1v#Wih;$1Y_V$4Y(?#+TO?#'3R[%'PDW-Xlxa[q#scD4Pc##D15jB.Yl>#13G>#F-&r0%V`i0-R:O1k`qv-"
        "5l9f)RID$#m;M8&;=Ljb@Ft6JWCBa<L_G)4cfS7XuGkv6KCX_P31QIn+M-@ADNEn1*xwTH0NU/=>ACjqDs/)jYo/VHJpt&F)u1D5EeL5N3at7:@e?).oBLf<DQCC6750XSED-p-kF6I$"
        "Wmi'#Kke%#hu_Y7c;,##h)j2/A@d_%)YkZgY/S-#%vMedLSx1(NpV]+B0JA,#grk'p8?V%8+i?#5,Eedp='JF>-[X&,rvW_n]&5`AU#*Kx:uTCHw7rLX9V:Lic@%u8vK*qTZ,8Gb;ktL"
        "v6;$TCe2BTO'?mt^hp<2LF220YUKDN<51s$$&###`3vD3r&A-#GXI%#xnJ(#0cC;$?7:,DJa*%$7W(v#8B6+#9DNPAB)j]++icp'SIuM(wSCl0^CtM(L;/d)B*?Z-0<3jLG?7F4>U^:/"
        "V5T?u5quX#ZHOHU:/1;HPRb#SE7Q2BC)fMbWaUq=;<$PAEbPqJPE(6<*'h>$E&hK->en6/51`$#@ODmLcgX_$,e4TA+DlY#WLgA#wuQ)<^=?tL1n+=$XU;uP%2jvS^$t^-=0DF7fne[#"
        "GIKsLMFo8%&ORW85%vl2bRVS%S<B^-';P>#tGuY#Jnl0,xr,%B,hn0#kD&B+$2+<.Df*F3,#=^#>qW[BEgS,Nn8YCj25ut%[mYv.->G##5YSiBa0vxOjYeK;HfPs.27gfL+OUH/V]xL,"
        "WB^kL<LLiumWj/MkGA8#+-QtLjp'e$<sC$#mwb<%<L86/;PaPAm=0p7b.f5';/[HF^Z+9%+jic)i@ie$NawiLEj7&4`RYid4W;1)l_XMBw[K^#2lKuu,XovL-H-##*cnD$]J1_%aQo(<"
        "m&:@#-Jc>#+GY>#t<RG;@;P>#uGY##0c:Z#4p;.HYt;v#7k@>#7VTB%B2SY,sj.##5+qF*v14.)O_es$m0VS.tA(l0/Had*g*&<--Eoa$b.)bR8,_D4pD.T%o@YI)N:8-<r_GZ.=k,?K"
        "D*(b8@P-)*.@8?[p=NBVqB[b+]MW7SY28?NR3jEu%?:EQ*MxK<NJ9M0w#)3#%)###k1#dDQ6^L)lTai0<om>#'$g]=0cu>#rM)I.>HFZ#IV@T.5g=;$?N=)<WDZ'=llI1(&sql0wE:a#"
        "KgSX-Fax9.F4e[-RJ,W-XYqdmt2Ts$uO0;0n8PB,sKO#DIq:Bu38-DOehvOSRi%609mZY>S*X[]xghU#076_u=['^uLD&tu-v1onbuJ98elXatB/&^+<51s$;m,>>vFH?$cBO]#Nt5tL"
        "r?o8%r1:H'f<uO'tun+MlSqlSedBh)n.Om18;5##K3SnLmAj/:SHAx$-GlY#E,69%Bv`]+FBZI)/X:hLnIAL(d.Yv,D?\?-d<<UD3Rg;hL8x<F3vI[w'+*x9.',8^fB^EtXi1DEJO-8$s"
        "^75ZT$*b:H48]`N$3WM9E8pVI4_':)Nk`9MJWh&vOb]]$s3h'#jG]p0Xo$W$3]U;$N%mY#vK,+#,GuY#<Dx-#9Js1B*&###dMp_%nPcA#E;dl/RQo],)5fL(a;-C#u)e)*Bq@.*ig'u$"
        "PTk]#/?-R/rQ;a4J>#G4;$&$&Sumc2Mv65<6nAYS<L5^WvQjMEBU9oVc1WUr5=]hNm43-H;M$qC6:aD@Uj?YJ$@'E<b;]/ceI^vA$(;ZLBda;2_;lLFgP6I>N7I#3whi4#5=RW$?/`$#"
        "c3?m'53oO9;s[E+$TuY#D3O]#G)JN1Ts-L(V_e8%VsnO(@p3hY5#DD3GuEb3kaC]XB/9q%PZcRZNkSi'%vxI:5+Ps;5XJFV)k8O#t53l8bvmwBUc(Z#S<dtLICo8%dCg;-8k`=-Tt%Y-"
        ")nXn*[e6##SmYR$ARG##REX&#lg-_-@S(Z#0PD6&/Mc>#wrZS%'8P>#;/>;$1x$W$jf$q$o%DZ#Pdgu$9Vi?#bC+Q'DnV`+0%qF*ovaEEvNV8&^`$q.X5Du$(&aedJbM8.e<7^2?r*JY"
        "gl%x>$Hv2*MG&Pu66-l.?:0%98@Fc1k5]iFA%Hc4asO+H9iswpmaK^ux&,HNCmk&#7B17#m4Mg$;m:$#dn'9&(>Y>#s5>##'8>##<HcY#<pi_$/:*W-tgn0#ibn8%9=;8.n$eERm;DD3"
        "JaB^#*D-/[*b[X`JoS(s,dNLu[YmX0$bsDulb9'#S1b(Nu2/H),'.AO)Yqr$C[6kLYcvM40$AT$=&A-#I^fW7VM,##'TWX&rU1_?v=`D+:>QW-4@F$K[/`SCj:N%$;:.[#=*U&$%r<p%"
        "8*JV2=n]r8rgZ_->S)G+(R)Kt=OeouGJQ=uV,lw.2VamL;6:5A'%8kO51%##V`N2BlxES7.wSd=0(m<-Zee6/ot7T$vC0sL10Y<8#b)<%MnEa,:$OW-#&n8^TJ+,'En@X-2dk-$UHMGu"
        "/eYMB$Y*]4dMo-?%#rEID/)FI*npw'WWe9DI3w9D:Gnw'5>g--Fx#.-KD)FIpAf--5JO2B;m]r'J,>>#Y1$Z#`VvD+t<w?0Q_e;-Cvx7(.pOhu4YD;_e)SqL:4CauH*3AOF8N2B7x6VU"
        ".)Xx'u3;)$?&g.$tC:TMZafcr6,>SRvx]v$-vIhLrhj)#@n*xu4=Vg$%6i$#Z3=&#:mx4(-]Cv#Wx5##&*%%%.`Cv#bj1Z#[CkcM'/>>#TiB>#xU^(+['Rx,+Afh(Yp2WHm.k**wSCl0"
        "b4+i(MII8%b$g:/XYIh(W/Du$e@0+*?V&C4p&l6/i/v-qPEM(pdA=u#FZpPXLnD#UJV2s(nw(cicDZ:d+q/B&<MDD30Gwx4Ovfq$I&aWu^3x7n3mZmoeXM^kbYnr6DBfYG<51s$;1x7M"
        "^HSW$GmNX?>$CB#38>##ecNk+T85##CMl;$AmDA+?H0-4ed/%#)Jo-)=WCh$T]WF3)W=P(J>Q,ExX?T.+?x?P68up3x<:5V$cJ^C.t)xtU$gK3YuHFV*[e&6hEkg#Wq6a.SmYR$2<S,6"
        "B^fW7NDc>#)6S>#5#?f;)5G>#:sx>#MWKY$DeZt&>aUl0dMSF-LVjfLmRU]X3#%WZKLcY$_ZD@$/mtg$H,>>#8.%kksMm-?&I^#.4`:v#9g<?$(55##NO)i%@DRe%?aUl0`Ii3=2b)@$"
        "wr/EWwLvN#9Q)KtmX`A.$K7st/^j-?EGMSf5U3n'fS7T0&5>##[h'-%+S4gLXPZ>#1O0o%eVdF<5d;<%LtCN(4*R@B=H,=$B^N%ud8&F-eiN7%=83.$9X-kb.J(RESfQd+UkV;$adMY$"
        "2S0$$;g1Z#]RPc*&g5q7690Z-P_Rd+CFfCWaeUq7f<]DtHQ^U#qO%f$Z_)##Vf.F@Jn=G2;V$##1]U;$=rTb$BvMA+#a_;$a3f7'?_+/(aAM4'Df1$#&De;%oxqB#$cfU%5[r(EaxI>,"
        "8FYju#$JGnPh/j>riO2B#vRrk:^(a$N;UJuZfK'JDEr(#LY@LEkT*$#GR@%#,7#)#2r_V$@:C,D9u_v#Ukhv$8/4*$;sQ,2XK^Q&G/IL(o7b**UdYN'js39&sDBZ-]p3J-j>Kc.gTF=$"
        "3Jt_$&:lD3+eTgKjY4uu6%BQ$o1@(an49`STIp&#2>j[jFa1T%lR$KuWZwO#I/5##(G:;$k'A-#H_R%#xnJ(#/cC;$>4:,Dt9IG;QabA#NVi20@M###l+0b*d[x[>B?d;%Ef[n&x**iM"
        "JH^+4O/DB#?2nX#V<4-U>dNt#JA'R@Ijax7n=GiVb_RVd/6LJu[`B'JCBr(#%/5##k#AT$hQ3V2[?O&#GDuP(+Ml>#,dG>#Ta0K-<it;2Tl###=-+Y-uV@D*pj<U..2h;-hgP[$J0x9."
        "(.ZM9M<'xT2?CYcA&K%k,GIR'?246133$w-3l&a#.[UY?voef$RfBSelC8Q$HJ>?_EBg$.`9@dFd`LipKt1o[H7=pVY`)#MH+n+#==Q26Qww%#+R:]$5I<9%/Yl##Hxqs$G2aA++GET%"
        "bJVg1J]###)(v$,fpcf(tW:hL%UCN(Qd>m/CDXI)vDp;.'MXB.H55:._*UC4>U^:/-AJ'sgfar`[-IlY-fnP-Zo=B8vOx20j;c%bKH(U=$qDi9cHRCjLS)V?u.6/1*iPHI%<lu>Be/d2"
        "Rx1s$S3L0Ye6]uGP&e4CkV8;-<^9Z7(,,##+;P>#T=M;$M9SnL%rw`*M[3K:=_AA$8&A-#Rv#Z$7h;A+nA;O'O'bT%JBbA#jAf3%4@n5/G/@m/C9+^,l(TF45h-_[,G++bc_x_?d#eOD"
        "_B)[#&m<e%4dh+H4wrA71BFluU$]O#Gq_>92MgL2?:,jQq<pq:Yvou@cH-h%G),##uCsM-ev`>2k=7Z7@q=]#?ksp%,MuY#>^iM1uxeh(_j>n&K/sx+sX'IZipq0(&4#@RC/k1)GG2^#"
        "R]CW6irxF49SgX#p%'6vDVpkDqB`v]>R[CWPJ-*ambJ-7XHJn$Aj%lu^X5jusYA7CKxhX#pedV$<D>1D4vV'-#.#9#dW`o$7NY##gQZ`*0'Sa*i.<U.Llr8$&mF?-H*Wf3rMJ=#DC6c$"
        "FYI%#':P%$:*lf(W?dL/)GuY#>m9F?:T%E+gYGM9:u7I*_%o-)8u0N($g_l0.bvU7RwOg1pn0N(b7kY5+@%r4ubBg[cwV&ApXTlu.?:EQ*qsU8x'8G)iBG]KaM3iKbBquL9kZ_#k%R4o"
        "*aP8IYC18.=&2[[wkj]7IAP##3i1Z#E&%s$9;XE$-)###AK98%OH#k*0<0<-q):W.E2aA+>tO?-7A?X$g2s]+b-m,2EM=;-bks]7i9J@#p':J):%1N(^pY3'Kk<9%<I@@#^2e)*>L)l("
        "jFC8.?X[P/k$b20+n<u-XiWI)d6BF,=LF;/X*]<X,jcoWGEn0Ycr2<Z`8Dv/6i8U9/3Fw?63=lSmW(YY[$+P#>=9iK8x0o[6xSV6AvPeh:RsnMNLZY#^G9G;iCF6'm2:D3F=*?#Oo@J("
        "0-k;-u=Oi%Y6o(<M]aI)Nn/2'QG7L([Da&+.uv9.$u:8.PZv)4q>gI*ta5T%B'>p%GIwou^2nl8vD@5&SH3BVxf6Rs$wFFV$/TFirJ<(cq^%XOE*b7Nr8w_s0F;&FCJHe+g<I>#=hn@#"
        "u6IG;@flY#m####'2>>#Dg1Z#'eWD<D%`Jst]>&#xin-).Lx8%S7Bm_RohO&q5&dt*ce;%*I4]Q5P#T=[>h?\?;r'*#R9`P8`l<puPYMWfHWksOI@Y&PgZ5)@>&SW&:YC2#EPOV-NwdVQ"
        "VU@i2tRKj26]U;$u,=;$.]L;$_[`s$NJ]n$L>>##:FuK.<Eo(<nQfi256i/)WM;T/$g_l0c7+i(TL.s$a%NT/0@[20KJ=l(u3Cx$:E=veBv*P(<A0+*It*mLTM4e#wn)Z#YP3+=AZ_aH"
        "6[l:CHU]N3T?Z%1`HVb=/5@bYh)Pv6*HKW7LJ]W9?e$tbpW[QIp,0kE;ippnA#KKinCqruGtIagX/LK#GV%>I$EcQ`p=>2''v&##9F.%#pkErL20?0)$)3>5H7[>#)5,##;uk+D0%RS%"
        "I[_,DAvMA+)8w8%>/>?#Pdgu$f;`B,i3/%#jeHX--&9;-U_5-0VRG)4m`0i)pB=A=T]lS/`U8T%`j(B#8u>V#%WHD3iv,Gnd3htq$5;`APrKJRcI*NL>Epae$/rmbl8G4L^CE0#uNN`<"
        "b(/&Xo%PS.=3@T'@fUv#Bu?s$Rf:?#?,JH21JG>#C.u+D2uUv#8T(v#'3Q=2AVs1B&&###WCse+?(*?#q[@L,(G+gL70/W$9*R?gs:t*G6#Uw0H1?T'4Ao-)f.9'f/,j8&WI7t$5>^P8"
        "_UO&#QvtbH+F5gLOht**(0Qg::QjR4F9FqubtlF[xVN@#6rL^I/W7e<GD8f?P2IouGHK8O@wgj:,Zu%=><H(N8x0o[6Ywx4W@gi+qU/N[AE&-qel###$wfx=pW-(#<xL$#`%3'#0GcY#"
        "?CCGDWIAE+*sLv#B#>(dePZ3'HZsS&1YvN(_YA^+a;DD3quDnW2SZ>#r1`B#/>G`aV%mlWp8eoIJ-s[$w>`k#P0t?MkWpucX(d6<N=6##rG:;$9(A-#P_R%#/wQu%(>Y>#(d(v#3A5##"
        ":.1GD3S)T.C@@W$DO(d-V]q<1QF<eH@[:hL[pO9%LnU/)QuLs-f%C_OZ2#H3j8s[gWfDR$c)nU#;p#X:+e#L#<R6s#(bZk$T;=YBEdu)M[st_$7+@Dj0](FROHfERjAgER.bCG)(:)H4"
        "6+eS%tWcb#+Pu>#.P:;$EGET%*c3j$`qG#$c38W-Jve[%tI8L(cp#7&PKmH34)h)486CD3TZ1T%.E_#$NuH/;=ukXub:XFr1.AUWX[1MX56*tu7NJX#X1>fu5uqZ#Q<fVM?YKX#Bs*J-"
        "wj*J-Vs?f6Tke%#>H*Z%5l1$#u#]>#6%DZ#2c_V$Ku]Q&-c?XAscL6&9r_;$?N=)<VLc(+Q8@D*8kU-Mb=r]0h7Yv,uR+F301gfLeo$&Ogch/)sq@$TttL#ZIT.s#ox7/dk;'aZrj#+]"
        "i&0kh)^bV$mQ5N#Y:Gfu[kAe%^CrRR_kf1pvRW%X1MvP&A7rr$.;R]4^g,A#>_R@#w?IG;3Sc>#ETv90o3g*#V4FcM(8>>#6&S-#21+Q/Z31%.2ndk(b`J&4R^Ma44j19/OM>c4N29f3"
        "p?Z)4q1J0YmCaO:J8GjBKm?d)haFx#>AXFr'-(b;?b=<Lf@fgZ+K#U#$WX2$ofg-4qF(Pa4Gd6;,Kr?>uaMlVgx0iMZrRu'(cq*RX^Qulc9bu,4xpaKX,,k'Aoim9F)P:v>u####rO2B"
        "1h_rZjnjQ/_)+##]vlx4AZpHW]-gf(_('<&88YY#aZ/I$'lr;-ON4Z-q;9:pe`@^$l^0W-%KRn*Xv@-#@Jmf;erMj9ih?N(d6]8%:X75/?;Rv$OW'B#K,8C4*^B.*fNkU(U4ON1iSJ>a"
        "F$>R#=g_`Fg_9'-qIVp%_i%lJC5xbD+P+8DGYP:veH7<%noBB#5uEk5,ur0vEm8*#6RiY#'PCZ[.P$##74rZuRPuwLjV<$#b^''#WLWW-,Pl>#5#-##,Ro;-)spg-Ll_?Tr2/%%JMJ#,"
        "&TcY#5;XE$q:^l8m(9v-KXc&#x8W+N_kHm8$C_s-9P0P'Tc08.MNo],tA)4'j&-V%6uU?#U/e)*xg9^#RpDhLGCAL(OJ-3:NFi/:XYIh(crxF4_mUD3*VE%$$%DJ:vVjfNYY6lOIV/&6"
        "T?1v7Zm(]>9COHX@[5YTL-uU*?[[`<E&>#[ZK;o[]#j=PiM@@#NMYS.v?mru,R`S%t:&lowdJRNO@K.CGY#%kI&*##;XR[uxXWl$Hf[%#.>5D>HTx9&.i_v#<D:8%:X<T%/C'8%TWg&="
        "E?`)+iCJ@>Iq7p&IO8:.8E_#$NeSU.=b01,hXqt)c6Jf:/H3>P?/BuuiqJau3`]]$onBB#S$###A*b@r3w]E[3IdE[b(cE[2[=G2&L%YS*C>,MO^Q##B+5jB=sb;-'7U`$@KU]7d5:U%"
        "<poOVPZCn=4HEvQ]W221YeHHMRb5/>@ISSSSpKK),R`)5<xs=].[c,M)uX5&2iC;$E2#?#9)?f;09Jk4YjGY>vm^N(W-F9%*EQJ(;UK#>4GED#xw4Vd=IJ`BmaU]=Z2*L#t?c7R0?-C#"
        "_eP[$G'2cuY(v[$(kjhupTco.rG:;$D*Vx04g4l(,V(?##T(v#U<*=%:Ax-#+,>>#D.Q_%%]Nj$hv`]+D36.)18TV-Gj`6N$F_<.IvsI3N#G:.k3K%[2:lKVV;I,[;]_O#bv7V?sA.q#"
        "&^mG$EE@Xa**X(Nw+iH-]whH-[h6q2Iv4X7ZM,##uE,+#(,>>#lG%o:Thm>#;,?f;B]ud$R_AbIehw8%XVld$jL3]-7@>U8RH,X#0B5DN2m(G#lP8OFS_wMu&l8E>u(%O=Gu.lu`AZtL"
        "qLW$#lIj'#hoao7Bf)W%X-+g$f)8F'GL>%Go.;I3^e;v#'mbqZ3#%WZ]Mhc*)F1B=F$1'#VQE1#d#4$M?u1-Mabxb-&JO?gRc1Z#pSB%%;vxqDI=%'OF8f;%G*Hipq9*/[U)Vrd,Qot#"
        "'Tmc$?@Un#%ljF2'bJa4xY-)Wak>]-HP2T94fUv#Kc$s$?Ml>#Al$W$l;5eHQV(v#9>t.=gvI+1%p-W$7m=;$BW=)<bH$[-8YZs.888L(YPMbni:)<-eOCb$9Ol)4]77<.`[I[#b#)4["
        "1momB+8GQX,IA`.S85K34%+?@?e1rtaQ%74KZI?Z3[W8T&po=H@`Zj#33ln:mR5rT9F0k$,0;W/kK)?$m[80OA/8W$&w>^=9_&6(*IuM(8E_#$@^Ts-Fv8A>&T?dFi+*`#(S<G)0sP8I"
        "2GR]4Nu.Q:2;P>#6P(Z#^fqV$wI6##Jm2:0Ai:J:,]UV$CX$dDmO529(lt&#5Ds1Bv-q`$0e5A4Xhmc*^E9q%nm=G%BWc8/KJ=l(JUwq$pMdnCT*#;/k:Np.eBf[#u6<Pag]`G)BH3P("
        "^/[W%oqOiX:mj8sxRC7=@@?,@67vGYC/hT7c@q`AowDXL3jv#Hp-&X7RY:xZbF01*G91T%xao'9RJFnD[t<YsX@7p_?k*FAWiJhuP?3RB`Cg`G*_:,)'YIC?j^-5HNMMiKc;k]Fn^KW-"
        "-V[Z7j;G>#LB4=$IP=;-AS(Z#J]U;$vp[Y#+GcY#whB6sQ`Cv#/r%F.-T%gLc.ZA#AmKS.8Yx1(?`Y##5)&x$&4kUI;+E6jJtY^$ki]+4v.<9/1MjUI`QX(50PVK<mvUWAs/=r7j).2;"
        "sa6&NVKjUIkKOG4%1d<IjGt,-m,<'+LupYIn-pP#ex0W#/MOfLZw-##QlYR$>'A-#CdoW7Vi_V$(AP##<pRAD?+%adPQ-U&[I4T%Uue;%*udP/L1oguE-@h(gQNiuB0?\?$mI#mt>+P:v"
        "R[$##e4O2BNs9SISn-x'_xL$#uQ6U&,Mc>#6;P/C22-Z$sXtr&bwJM'Q)S5AVHKZ-k;DD3[UiS%MRolu(:M:vjnYkaYq2>GB[te#/^%5&OH&i#Va(c#uu2JMt=*mLQc'e$muB-M)U$R&"
        "9x$W$Rsg-O2k;v#&*sl8wCm92f-gi'<r6hGpVA^++.h4.wDN/MeEd.51^INOkH$YY/5@R#fD',d-^,,2Y^?K#(ZZE2$:V7R4p),M$IbJ(P_9uYCYJ&#SR=/D-jTh2B:sr$-BVc#%,>>#"
        "#-:kFdpi_$1(R8%e/SfLdvi;?U@kM(]^md+NO3]-a[Sw>PvdC#%8cY#1`jW#iI%ePQ-hpPnM$4?XBXMCgrKs6_cCY7s7D;$.JcY#vH,+#/f-s$D7HC%*5###fo?6M_*FK&)E[`*;B=:."
        "Tq*9%d$bT?Oj<x%j+>c4'H,=$&Y;MLt1d;-fLD]$E<fG3W2a;?-4S`tEaJR.4c4JCMK:T%xql+Duic>#<nJm&6iqV$nZ0[[5lj]7GAP##0cLv#<6,##5B6+#r[)E%B?:X8Qp<V7m#A/M"
        "VQQ>#oRa3NwRH>#<x__?]Za9D.vsx+;Esh)2b<t_8v%C47d_m/BkY)4[]d8/ljMm$37J[#+XOx/$S1/E9]ep%fw?'[R(C&49XlW6AL%q$nar7'4U,t^^>i?u,Qk*%7pQfL1d;xLd=<_$"
        "BA%%#*$*r&(;P>#]60I$NWI/Cp3+a$KJF>C6p9dkPZiO`hS2K(cxpkLf;/Da.(FDKD-(b8tT7?[W#Codx.'JhCLs8.DJ&L<F=r;-S%c82d1%Z7#8D;$5cG>#'3@@%D11?$<FqcDE-`^#"
        "4)S-#^eev7&,mx#11CG)h>;O'GX*9%2JG##ZN#V%1dKH3(1M8.5-q)=_[MqM:qtBhj4$5K$>dEQJb[M6P.'c.'<5F#&0k+.@=L+MXn4oR4LLfLd?7##hs7T$dV_/#u,_'#@6.B$.`qr$"
        "EJuY#HYCv#k_I^0.M1v#A@d_%vtY29?kZp.7m=;$D^=)<o[S60tw431=$Ig)p649%]CXb3X5Du$0SEF3f0o*5e8(u$+MWb&B&#v#=%vr-OZ0^#CQ9(,&7F.Dx^OJcCnsr#xsU($S78x@"
        "OCFf4`_%pu7mMqMR0IDRPNGR&:S9=8a%Bu.hkxad2o?ju3uwx(Xq'WjID`JD9X687BZ2DNivrsunZ0tm#)P:vXW#A4fGl<C'xWEnc>cJ_mSch)bl###v<*<$rZOJM^@HW6wb8]XV.M/1"
        "QN;>,4-xA61Ml>#@u?s$<]U;$hu/L,k`u>#=pS>#5#?f;7+Rs$:0=;$6v@-#JQ0#$aUY</,nSV.o^3R8Gm]G3?*k8.v(]L(/x/U%&0*_XGWor?,G18aA-`q;r[GZ.MYPgP$%S2B$o85A"
        "[B/+jN]qqSZ(gf$^ZD3RkniuLUM?>#$JYCj(qX9VD*u(3Lej?#61`?#s6IG;0ij)<*4j&HXxd=.nbq[,igIw$?oDt.*<4:.Jan>S8euM()1mpN`r8rd*6+(5i/.e%P?%NODv85/SCNX-"
        "tKAk=[.d;-[sWF.<*rA$wZ,N0w5XV$-iqV$-D^293NVW^Lb?/=jp]@#[TVW^lHob%S.5I)b]OA<r[5?.`Gls:S):6Va,H/(rQ<p`a#QpSGXgc.=mYR$t=3E86Vg/)c1BY,b8SulTfwpN"
        "Aw$u%;5)T.$),##`+9v$A0u(3P'9@#Wx[fLmf&V-;-fu-i7[qL/WZ##GFg)3HD/^+Ce9L(;B=:._ix>,A7J79_$-<.sPn1g7]#A'+1weQA8HHODfDFV4%9_Alugo.XPJM_(T<^4QCrv#"
        "w9@G;/x?s$Z(J/L2Su>#WY;MKOT5,Na,g;-+/dT%&Qw%+tRK+*/Sh;-<2UH1$TQ_#,q^#$WeRs$oN4U%W1,.)'TGJ_BNH?@Y]l>#wM?U#H)/dW7xp__<4dhuGt^^#jYEAV,r+'$GADIP"
        "3VP:I]4S`tUK&c$,FH?$cd(Z#WJAMBg5GR*:h?.MDb$$vfI:;$K:xd#4f1$#(4MgL7_?1%KT9q8i7j[?at`D<bZCD*#pQ]4W^C6&SlRfL>C$##1]UV$>G@6/3Z]Y#j>^nLSKOg$vW4)<"
        "6_V`+;$8],Mb_/)wYLl0-M>c4i4ST%-AA(#PL.s$nbVV$kdSX-RDJ)$rQh;&k.E'R/u7CdXv)p%bp/*og>Sxtkl@?u=GJl#s9:c+Z^LM8WNWa>oQGxXsXH0[8l-Dqn^:+rT'a..R(4GM"
        "WM6p-01M-v%uF'NJW%iLHq>WML)pc$2kUcM#nsmL.33MM;Lq`$,@P+Mj&1'#]#B.v:2?Z$L(+&#Pr6N):w4j'/lh;$+AcY#x5XV$+]_V$9nfu>YfkA#S'=C&=x3N0N;8>,O9x8%oF8s?"
        "PuI4Vn&6w$?Ji`3$uoC?ffZYT&BGtq,bbCW;LLl8rF`/A6iXF<En&x6>KeEIGNwjQdr]UB6mxjbel(hF`x#j^up+<-e*fQ-YY?T->4JEC;[@_oeEl^o2aIR*/PVd+RSqw-U++GM1;M8I"
        "Hm$)*Da9a4-(u9)e@GZ$3A@G;iZ2<%,GG###_X%6T(wh*wU>_+4Fdc*xc.1(13K7/n>Ea*f[e[#_[0H2[7,i(SBo8%>A%$$k%E]-%xjD@g6ar$iJwZ#<kMvUeQ:>uAK;k#&lDt'V06Ns"
        "LTMwtft/&tCG.+Mg8Q:v?O(Rar$XRa1#5&>@5v+4)gF5/th(##CY_G`nqQS%3m?S[5QHo08n+6`cJgaB$Y[]c=EpkMe^5wNM1EqO.2DsHOOdcahH:@-)>m]%69dwKx4XR*qj6R*'D;_S"
        "X<k_Sat?>#PpPr?r[O2B?#->>f<f>#*DY>#s6IG;1cuY#]%&/L71wS%417<$>;ur$8p.-#I7Ox>v%CB#:rP?&OnYkE2$Ra$S3O9.h2MS&8?qT%b02N(k;oO((DtM(6otM(,['u$A^v*G"
        "Clq88h$PGd2Nvk(BM<;6R/1sUqQx;-.)X:%,_I1^HE0ES=RbA,L0XMLE$uE+HP2T9fvGn&dhKDNRcSG;/`(Z#Zr%/LeT]s-W5^+E89AI*rlIL(YTpQ&;`Y##I<+P9;GG)4?9^O&oNd#$"
        "20Ou9fB-j:&&dP&4mvmu/'w2NV6b[%sK3&C)e?T.lF_/#pKlB$5H5+#4av_57@<p[&wD8X4tV'#%/5##2?uu#IW@b1CL7%#pU&(#+VCv#:oc;'Unv>#4;WlA5mbV$iFV?#Mj>n&e#H7&"
        "DSN+0ARE9%7>-[-QgXV$K/oO(kX5H3*;Rv$#VB+*S,2Gn_0okpq]s@F7RZd[#C*p_hZ's#W'&OP7VMxT:$V(#WjXj(JwD8RNm_p.mvK'#lsZ?.M%R8%u,=;$0cC;$VYB>#.SY>#@s]Y#"
        "@>+x%9#4kO`]-T%KsQ,2f4-##cxiP0QEa`+gv>n&'u]#-<$em8mdu8/K)'J3w4iK)J>#G4/]NT/;V6d$bf)T/dYSF4<LPf%apsH=tNttktT=>0IF/W$5%%W?eAo]:T)U^?dQa)R]R?;^"
        "4lNu#p[69cC48Z:;DHVJ'SC#FGfPi>&%<sW3^m/#$),##Ct7T$js&,0u1%Z7w=%s$GX:]2(>G##Ud68%-G>##<dA>#APCS%x+ffL'1/W$oKfnLbJ;'#Z/-o/<J*W%/'[0)%=bD9;KB#A"
        "d]i,)^fY&OW):</K'@+QWb0e[;ZVY#<*SjPw?[g3_Xs;$5:nS@T;G3XU0KD<9FEVMPn@x`?Ee^u<8mr9&w^vI^#b^8oTF1OJ@M7:<TOA>r=wd*&.d,4@M1v#1N*$$.iUv#D@1LaPX;;$"
        "DuUE%dv`]+&ef/.+L$iLPOCe;m+:B#xNL,3nKA?M'M/d)Ei(?##fpL,FLNhut;PXRVZ%ku>xboLD&#oPvtv?ev*8F/W#PTaJ&KkL0YO&#.cCvuR7Mg$SRk&#:x7@-(;P>#*DcY#vSL;$"
        "+DP>#.U>`/-B&##5B6+#?FA5B]UpG2?N4u-bPA^+v/tv6<gSX-$6:e(np*P(Qc:Z#Ye'VCU8Y#-r,(m&wlH6%K=6>SZsrt`_w1*4k=l/,YAE6eXnQ8p_rcI[#96G)6vJ1TsPZL-wPZL-"
        "f`0%1,4o<;1BVC?<.88%m^i;%k_`*/.oq;$xiPnqa0nY#<Btr$>f#_%DD4;--Xj.;;OQt1+b^v1Gt(^#jZs;-i[CS1-.4K*S.pF*]a5n&l(/CQkGPA#u^5[$@CH'7jX2u7ub_s-=MD5W"
        "Qw;MZmR*5Gn'5[$4t*W$*$O%Y$^GFuOn%HuhZev7_,u&##)>>#nR&f$TMY##F####qvdiLZ*6]8$UU?^PJuu#Ur./LjJCp.?mDA+KhW8pA^3-*$W7L(xf_l06RG)4Av,T%m-TT%AAC8."
        "rZv)42=*hu+7=>)?P]l;rOG$%1q3W$tV-Z:om<d#28.$T=&e:N+4.AUSg9a5;%7ce+-H1W<bF>#S/8kOw[?&FY_m?$+>P>#OJuY#iRgiBdI:$-+>+i(W<tT%LeA;%Z(2<'%uXI)ag`I3"
        "QS4o$:)TF4&]r;$W7`d3J^Rl18`h*%N<ciKW[6XBdXkuKQubR<8A.YOJLQRFJaEb?VGiVV+07ZSB1]6<ESS8ItkTb6D43P<p58&Ki5<W;9nUpC8mPmEvjVQ&OT[IFP%MfL<f5]u9>RW$"
        "L(+&#1d=1).oui98KMR:^Ot1B$-e<&*bd(++GT*>>qmM9'%Zb>G<ie&cA6X-H1;_dKpbD<jS$=-B,&n$o#fs%)))&b>3-,+dGr80X-bT%2`:Z#2G14)p6g*#55<PA]SBg-kg[B]g3O]#"
        "[kn0#q[e8%HBSX-(DeC#Me;v#8A^P8S-x;6=*U&$X-?JLo+qkuS&u:]^IIP#rJXD.,_=o9Nk4P0&h=X$$Ift#VkHKuR&JHuM&6V#PILlgl]<EAH;<j$L/5##?H:;$c4ef6Oqn%#>TW;&"
        "7%7W$:o6s$x>tr$.o-s$CD5?#KX7CSi3NAFePS2)K?$)*xRt.)O4i$#(`Xe)oS&B+qF+F3?DXI)(Ro+/qDp;.&1Nla(C++Fr@OV#P0O(WCe4nuitR@;QS43;);of$=?9C(MmhRunM8a#"
        ")bKY#^e-i^7*JcQ35n0#[e+/()O:;Z44fh)6qVf:W_I>#$7@G;RlIfL^f2,D7Vl##;@es$@NG>#Or'P%Su$W$OMl%2MWKY$ICA%#ltT+*+$NSA`DHO(nsO9%J7fC#.W8f3KfM2:JGTZ-"
        "8*x9.[gxjM2mJ6(xk`&JGOquP^bHK#$?nS#S>8OP0Y?g>lqxh#A,Q_LMv@iuQ^.A+/i'Bub81n#5;>LPQ*P:v$g)]XE]Q8I+6,>>++m<-;a:1._khpLGwXrLl<-##2*m<-4fav3T>cX7"
        "-?Mk'O'tp%8`uY#9Auu#NhF(*3s>CFL`'d4%F$`4xK^:%v%:8.udR-[q#rAqnv6X`U:&LjKT*6&U_J3GnTS4fCUvFnK/MwK;0&tu='auLL0%Q0:Qt>72Gc>#L`hV$B4)v#A(dY#wN,+#"
        "-i$W$<S4.#l;Ig<mm/W$9'xu#4s@-#GEk]#cU*:.R6*H*?aFP9gLuD4fs2+=6j#<.+1w9.$4Ns-?i.P9mxe:/t7;HQ_wEFGDfLs7Ja5N=V;Wh.r=TW&T(uK/pQ*KuX6C<2689(++RT&V"
        "rdwD$6LKw%jc)kb-Z-kbh^4D<=9[gL.42B#E0x@#Dkn@#v#cNbV%RS%Dn?dDb9pT.9SP>#=JRM.CuS2Btir^,xd%##n6io.6EPp.W-F9%-VjSC<TnO(%Poxna?3ZuTDcP#0D_QB,>PQX"
        "FuWj&L9Os-TZi2MC]>-(F7Bo@bG.;6DBfYG;0@Z.k_Y)4x]-i*rZDXa-LcYuDAxQ;&>K2dsia%[][X5/FHGbKW,[X&bt`xFHtEd;OV8>,8EX#7ZF.s$2YuY##FIG;2.Is$^.J/L&;cY#"
        "Q`%/L*>>>#==:GD0lLv#3Q:v#7t[Y#;B+)<H-JX-rAC:%t1rB#Wah,)^m%iL7CHZ$.9]+4VVdG%a,2A$#DEIX7NOON9HglA6te@ujSie6.ZhGc*%a&$/(to4cIO0/4`J2B?Ffe9(e7k4"
        "=^x$#xgA]=1A?p%?VQP&63o(<kp'@#BIr?#TN5qVY+@8%_@S/L+j#X-apN8pvg[?&Ni4Z,;u7n&PuJ+*p7TB+'`kB.`aF9%+ko0'9,th<e]S+40h'u$,;>^%bo`K;Ls^YZTHl0&I&)ku"
        "8fSxGoXY8N#w(3#&8Yuu>nYR$s;Gf%:uho.N]du>kT+?#Ek7@#%:IG;8[]M'-JYY#Wt8R&7:no%;=P3'6&Pv#>bkE+8VrZ#3MsPAH:w7VYb$##m4<9/LNo],^+9>,8Id(+h,Hm/cQ':%"
        ",QdJ(VYNj$ag;hL6r<F3h4j5/&ulV-H,pS82_`#56l*F3`GGU.<LrW-WgV'R`6Y]TZ7wp38]8]u*_ZF#uW#o&mWg7nwb[Cjl)UbFlAOQ#,*1n#qrL5/n6?_#O>KsL[VOjLTj,M$3`R3k"
        "gU[P#7]f=#YPW&#twiM/D'A-#5'niL[dSZ7O)MU5;=<5&#I,+#4`($#L:q'%,S:;$2GET%>Owglm&?f;185##3v7-#MWKY$0BUv-$dIL(5s(H*..YgL;x/J3QS>c4RqUN(@M0+*@[fG?"
        "tuL#$T4n8%p*x58[pFX$#I/[#.vX=Ikg+M#a$^<&8W9/1ghs@uvv9i#?4M?#@=L.hh+6xkA5qi:?0%#5SAS;e8:GYu'unV7X0.)A(+3t_5$.S:/1A=c)AMfUjI'##X:4gLI1_E>h4)N("
        "x#l4M)m:-OvN7l]*,)X-j]-dGLTZ2(,<6(##)P:vAUS4op;_D3ak#AO/4:^#L.aw'QM_`jlTai0[mYv.ZYlY#uNGF#F$AnLj[vY#6R^>]1GdA#>GWt.sD&B+m<Bu$@I.[#u.'J3;8XI)"
        "m1=Z%kQ:a#qn<:IX<'E<^D0I4LKG%IY4e_Y/-tpGh%r)etIu_T:@mcu>?8W:r,TWCW7Y]$EuL/;fA(MW#SR*vEk(co&T@5&WtMG2cNs11Tot/ap/GCTP2.HG%af)#]I:;$.dH(#Nqn%#"
        "%iwZ7#gcY#;+)v#1i_V$s19I=YJv;%%qAA=L>KZ-#a_;$Xtwt$,u.@#4v=^,',]L(#s%5'?7%@#X,e)*Bq@.*B2b;%=Mn5/t/>u-9WBe;>TbA#S>Mj9<sC^#qpP5@%k26J-BNuGt][E6"
        "?*@_D4q_@bdbmn<rjXS$g(<O=h^3rn`i0B$23n*NR$hOfchPV9U:d%@.Wh%#bJ7;6TKY*+IRVV$=cl/*V4=GMG?Rf;.o+d+>e9L(V8a&+NI1N(n^kA#;Rd%ko02S$#)U:va5ov$2w^^#"
        "SAh<.15N8=-0VR<%t0'#%vds],5G>#i-Xr$'#FB#Axt.#l2R%Bf`Pj)(AcY#r=Au-sNIhLw7]8%`w$*.S,8eMKo*@ZL15B#[@>XP3/]]uA9W9`vHWhLVMx(<o]>E7nIYkO8.L$gwEx@="
        "M-iWh<Q]=#QU9g1F4+&#BQSW7OJcY#XoRfLQW#.#8GWPAP,w,+5-;hL^,u9%mA?qV@;=^4;HC^#g2*[`m1$@uO]+Y#C:`%bWQ;#NDC8sea8,>lQC(:/>jTv7Tu-s$B7D;$/-wuSPL6##"
        "5ohV$]D90/-B&##5j88U-#)'#h.ET/AhgA=#OWT^/]PUA#HG)4e0I&%b]4qej0E[uG6'=DxF%=MOK:T%($9w@6Zw05uG=huB#jiW]j7'JGU^b+Xh]49o_R]F:FM<9GB?qM*UT`3/r2g1"
        "@.[n'm&Yc2B+n>#w?@G;7.,r&S#sp2*S8L(ba9U%Us.b40Rrl8<mOg1]RIw#rB4o9uWmQ'3-Lu;#=)V?'D$Z#(?5q%*gEm1XR`@0=G^R#5NXeutNNjLO?fWaPxX0>D&>uul1)0v**O=$"
        "_MY##-mIq.wK@5/erE-Zj8Ea*8k*b*;'$R/K1HP/[h,&OY9'h)xk,K3:>5##M:=U.JDvp%MvwA%^L7%#3[ChLl^dJ:Q<=?0.aC2'UxVXr-tF6G8lj9Xu`Acf(c;suorvC#a)DG#$O5s-"
        "35QAM7.6xLeSxqL48B<$+wK'#9KWV&17E5&:<hc)J]OV-No$W$qh_'/``L<-JPne%lW4)<iFWt.u<o8%^dQB$@JUv-8bCE4=DTD$8FLk(aO`;.x1.=E>R9m<cVdL<M:$5a]Hj8:A@s8i"
        "pk%(;?w2KI:xvbrixKF?>bnO26v$s8EXAE?Ggo;64P%]SpcefE[1ab?K5j36E@uu#,6+&#A?8W7Z.;Z#s3@G;.JlY#:QQF#P)+gL*FW$#jo[L(C8Y7%q+gF4:>0N(,['u$lbw7n6*)Zu"
        ":Dk('B^6O$w'ZV#+jCC:<7c>,e'A*dg9;,#R(4GMeU4T%di7)*qi]v$-kl%+'5is-j:>dN66&#Mr]-(#5g=f_dGeMB>[/2'gfu7IPu5,*2`l>#3P@<$v'`f:ALcd<:M=s%-JYY#Br,_%"
        "*ep'I`qd>#G?kx#@]Ea,n3h0,L8,##Sv-hDg(NI-`QKd+@S,##,[%X-@Ud(+vo@L(x);V.*<4:.12^R.P,A;6wP8*3:71OOFcR9%5H]YWKjJi^*:smJLjqbtF4VG#pYs;PS$+iKW,Mc)"
        "e-i%$G-j$8]+XngDBfYGP<kl&fS7T00Gc>#QHGfO$(^fL7@Rf;[;301LZ_K(iV7L(x`Ul02Hj2%%X'B#)@+9Y;MDh`[ru1Td]s'g7j9_>;Pj%+/5.Po7lP:v0VF$/66A5/6FnHHX9R8["
        "q6[a.fq%9$EKW^/H^fW7UJ,##K_Z8&7X439K56X1-DJfL8v]A$oxqB#4t@X->IR8%n7o]44cJ=Yi;d4'.@LW9NBwl1x2h>$#:;8$N-oP[]%]Y/MFl+F0dCYY$',>>_,P;@Pv#L)jln51"
        "I+Vv#Y.+,M=h?##RVi.L1V1v#WOTbG)wQ(#]5RL(gWbt$q(rB#-LI5/Qc`#$R?H:R:V?o'@8Aw$YgM2gl66<$?0q.VUwlIgZB@`#Ew&<E$0(>c=gaFu='f@u&a*9#HF%f$+?*oSnoI&,"
        "YuaB-uEH&%GnGU%`NAX-&?gw#<A$(YDHR8Y$)q.10x0P8nQ8:m#)P:vKtS4o&a_D3U^V.hk]S:DncOV-mpTW-V^%QsF/f#/0m48R(XS3b@idYdXl).?cmuw':Vg9D8Ts9DrVx9DvY'##"
        "^0q^f:f'_#'>uu#5Q(v#G319&d(x[>;ox=%4o0N(b4X>5gA*1Ynoa>,u,q+$bP:r@l>6?$IVu>#:x0@/PbnD$)?Na*3wJW-To'OFI5>##wSmc%W^+)jqpAJ1Zc1A=*;*^#[Ht(3Xre?$"
        "M#`k'.`Lv#s8YY#9Ot0(#0[w%8rU;$62=*$p=x20D?+?#:P&2B(5G>#mP=42v9dD4Da322xVUZ-qFs9/Hu&9&q(rB#3`@gs$Ii8./?(:%ho,@&*^B.*j;Qv$@]WF3)W=P(^;Cv-Wfj**"
        "1I1WH`c`^#Wx>nBi0tS2Sdm8C@WTQBn(#b7q9kk33`n>:&;c61Hm'w6:Rsn90Kn51UjI^$l0DV$dW<RsFv5@OgZse)ApWc4Fs,rW`vDF4n>=:lMWx_#..IL2`c@73/<xtk9%###=kM]F"
        "i10p7Lh.s$f%w<(W(5YYNK;MgW&),)jln51;JuY#^t7^&gPl;$m=TP8Sx#0)m0h/)bg.n$3Q,F%(Mb8/1Drs$O_fs-3^/M6jl,?U%l3Q;p,_>,n;g(a'G<IR8s0VHa_a)5j+P`EMd:5K"
        "HhDT&^#v;/Hxf&#U,o+Mb48W$p:QdHEawf&G5<30M?.M,^7@P#`1w</fp<xFwED%FNpG5BJR_G<VY*w0a=Rf;qPsp/vuEhUxOvN#76>k-uKOh>7I+_]-O[Cj5;`-?Ps'w0jIRf;:rLv#"
        "?;:m'_w=>-a85O-Y8&F->_CXNdN?>#r3`CjV3Bkb,1'B-(AcY#Y6,##7H6+#&14ZKunKb*vF29.69L#$d4b/1c&uku9eNX(;v>P#^7.M.$m=5/C#J'#%/5##$J:;$LTi].DL7%#[]]Q'"
        "*5'?$N_u,GSsBR'AhFJ(cG`O'Dl:$#O>g^/$+C+*[L@h*+J9f3jTNbfQ(&DHQ4&JCGDSSl4,2G$M1_j?PaNTV&`EN0CD68B4*5/#%R^sLvV$lLxx)$#qn=4K65QgD9u_v#)bBwK?eME."
        ")PS1(?MSm')9x_maoOB%`SvJ;OX'(,x;a&s=v*/_B`I[<T3@YLJ-,^YBGbkZ`mBm/2Y<Y5A+)WCs4pr-?IMmLC]WmLcllgL/6Y7Kcs$HF/SA6'[+I:R>=f^/W00Vm.CK;$[P%iu1KTD3"
        "j=F7Lf,tNOx9LUaArC'$rff(3::m2CcHa-#,i68%^SQGVtUWe*/uFS7_Q9@#w<@G;+UME.0F0s2Vo###@64Y-w`[`*qm<U.e.QeQ2DXI)BvV5KwYX,4V2Kfh-uMln51lH'F'JsAH_Uc3"
        ";24E*R_Wp/fV+sLReo7XKX.3$Qe`8ZhR4Z/WhUGF&0,AX2Sw=5B1DsC]x/ZQ#oAAX1Md/)207--#86L-&T6s%%1/%#fObI)sg+&#Z/IL(ERcs.sl]#,ULPL%Mn[s-4(u]>a?<`#wW$Mn"
        ";?MNbm<dRXf`)M.>hpF69#fB0YclGJbZsc)Gpp$'.eWLuN`@M+E@=/>XT#N0I$gD3i_[7n;QZ`*GPep.sbnZ7[76--gpA@#?XI@#'L[G;K(o9.@0=;$O_GK1rSVk'Q9B6&ODSY,7N3*."
        "%4;W-O#9+.BF=;/ND#G48d2$]5Z(nLXp4Z)x#IY,^b`6<.aXR:eopQkE8$S$7^mfEw&=4Y;BJ]1;gG#7:-+xU2D/hY@=F&,F0CF+Bjv%+8[&Q&3@N5&Tu[fLIKH##5SfC')?<_#2p@-#"
        "FEbA#*H8MBQV@&,o0:a<CH%U/+_D.3<qH)41xw/;^gp580Ufjoo?=_BV*rRLKrh('x&Z.qH]c9;5N4r9s_H_u?%dntGBscFo>HwM';i-3`Il%vk)a<#4:[s$5MY##`?'DN@$58nKC14("
        "_(8V?LgS_/Z:V_/qC$##@3RJhZ+48]l_G_#']R4o$[bD3W+5;-2@EiLTh<mJm.J&,@Vo*&>%u8V4:(f)Zb:D+9*S=-X@&.%_`Y##VN>7&mrqB#uGZ'=Y-M9/L0x9.VF+F3hi'Sip@W%$"
        "7Okv;ZK;vTN97OX`x`sZ(vGR:N-Fe=74V@kjlBUsF4`UPpt^>,:<-tta7<ZanIq*goTXR-qX?T-@QZ&%Z.d,4+VL;$#<+?#*8P>#;Ax-#m)H:9SqVv#+FS29'P?O(iW+9%X/O1)>;gF4"
        "@h=wTkLZ>#Bf<T%w<JF`HaYfL?8arlUK%Yk&$pI#L#[`UP)ZV#%Uke#hiZ,?FxY,#J^0^uq-Jp$sSk&#lFJB,1]Cv#2nj2:*l)@$t;,##,>cY#6>x-#%%c_$sW6+#JK0>$Rs3I)B88L("
        "$mhl0p<6g)'*UT.@Tsv6Q>oa$XCn8%0gYp79,B+4>e<A=peE8^Dj5A9CEe89bgUYA>o/,,lcth$#8BhuQw2iW)Jl=R:vl9J*kSHHPC7'@'Y3BuZHw:N&Grs^95GH$KiUv#h####E:L/#"
        "dRJ]$T@%%#eiLY7VfWt(Jk[Y#94n8%Ar9F$&,,##$5T*#YKx8%qU2q`_H4)<B:VZ@sF8L(628L((-e.M)Er8.2:WS%PBrHM0+ho%3WQrd9iBer==K;$[MrhuMMlP#QEQq<3cOg$EqJhl"
        "T'wtUD:e:N5%<`uIsh^@TO&##g`G/vPfYJ$=+Md*qfEa3AVcY#/G,##DL`V$HX%s$>Cae+qID=-?[D=-7FD=-(ind$/PZ>#/cC;$>4:,D(>P>#D5VZ#>P)g;31Is$a:dL/HjO)<2e3*O"
        "x-E?#IJQf;GeG&#O^3W%N.RX1#>F,MkYr]75####'A9N(TM+jL/e1N(*#%m0Qf5x6R4r4&A1(kKPQ9T7Ihs?'nIX0>#Sal2`I=&JkgjhuBaUoEFuGw?U=1n_<MegY1+Ll80rc/5lj:60"
        "YS8X_.0ijCS-^S%6S)@$a'nA/n*^*#jFeqL#$4$#FdoW7ScUv#+]qr$?;,?#A>Qf;+55##76o(<M*3-$XLH>#(D0N(=dS&$bwEx#XE:R#Ns>$L#gmKZp2%EQeI^>,HBx1Tf9w-$Mj+DO"
        "Yx'##f.RqL+PvwL(+'%#V>cX7_PG>#7fL;$>@UGDFN>m/C@@W$H2s]+9uFc-bs2(X:,Jp&G0&%@PoUO&b<4:.lZTD3nN#1T%C(JWb.jX:(04^fPwOwfrqu;%*<2uLQFP#$Hu$:M#w'kL"
        "':RtLWD'RJwDJ&,W$wt,m>*U%'oQX-)9T#rFS6wb-@sFS]24P#4GLlA=PT`Epr?oRJ>j1g-;u-$#hh2:lL>M9k0sxcG9]8r$>Rf;(;a>-44u?+K9uO#CX->c.A=AV;CuZ:7V-en@6#mu"
        "0/+]uLGZH.$u2D##)9tUhHLfLD84ku9Y3t$TGY##o]:Y7t1dY#?4D;$`s2U-aE+)<AdUh1kvpW-@7>sq=NdSJ:*[)E?DXI)D35N'eL0+*jqoL;2V0fu@8/=P+M@x_slDN<m*OcV[]$d2"
        "D$hSW=^DFK,gS*=I1C'>`[+V^iFnLXowCL($t[5;9T4)Qv`;NPL1B:;(H>S/.QVw.#)>>#'[eD$cFb]#j5@K<+ls`#B$###A(1/#*O=M$Z5i$#rBh9&07WP&X(ffLn;=V.+DcY#T[FZ-"
        "#gfUT8'901j$<U.e_e8%4vlA#W%hN%10o5/vBo8%[n#ru6dcY,S%IZA8WwD4VSd8/_:gQ$j0N-$n-xQ)1@jN0c8F*vAh-&/.uC5/@B'O=N4B&X-43Z5&2P:v;<+,`jDhi'h6el/2O(<&"
        "G%Is$Rs(RJNChG+0cu>#6TuY#92Qf;6r-W$8>L8%ED4;-xI>V7SHN'%IeA5BJTFs-t2W+N(.?jBU]J]?c*JA,k/^v-Yfvo/.lK+*]*49%B3pi'?+=+&+'^s-4KDTJdo0^#/FY_$]BTv$"
        "dB]@#6%$f6s9[1FZ'-gNHQ&<VVv(I]>Nax_3-l2=21D.=7)lbN^bd6t*qRwXJ`/iuZ+mlK#sI%u#kgmt2:_J#6t''igsbEhC+,##1lYR$M5+&#iC@Z7@$Mt%?nKU.NciQ&gDrX%#Qw%+"
        "(.QR>>;+6'@Y-[-$GTo'cDvJcmAC2'C^Xbos<A5G0>3TX@tV@JNP9S>[%b`utk(G#7b;su6)P:v]M=S.i&_D3c`rc))Yqr$gBYfL*[AN-M^AN-pbQa0TV1Y7eu$s$I3SnLm9Wv#<Mx-#"
        "-VCv#Ek%?e_^+9%JD/^+.Th34h9,/(w1tHZsox>,U'f8.YwH>#j?E].fEjINgjtd=l+6IQGOp(*MJSr0GtQ&/^u_Y7c@&FIM;G>#`7+gL7d#.#1%@W$Bu9F$4:b&,bF?aN)Y/%#BIB^A"
        "^sfG30D>8.)`OF3Z]Wj0*cGg)Z02N(ZCZ>A+j>V[[t&2KrW(65RR`>,Hhr4n<ffx+)g1Sn*<m%+Gk^m/rH5+#:]0b$hH.%#&nmq&)4%g:3u?s$3+.<$?D:8%+f68%>MUS%a,VX%T^X/)"
        "2Slo.(#%m0mx9kF?o0N(9,B+4P=6g)]gvTM(EeS#.VL%X]p=wUKuK^u-;LG2l[?Cuh-^TrG4T4Nv12AN':>YY/(7m'.ZWSS3N'R'rF0N24V1Z#>m%qrR](Z#Y@IwT`^]>#92Qf;6fL;$"
        "?N=)<?^=Q'TVRh(OhHW%=P-[-gEl0GdNrC##83DW*cr-aZ+r.UM=%=`CWsguJxTgLcCBluG[*rL:>eX*b;f[%Q#R?-C2I.%<^/M,*p*<?&4I0YNqd4]W665^,7MN1;ivu#_t%Qa3ZS9>"
        "6+qWhWUrM(4-u3<.ie;%7<K[#&R=.YbR#N#C/veqnhfDOf@*rLXA?>#667wgUQe9DvG6PJ`4'JFEOuM(8E_#$O:xddaN2GWrvvt$#Ff&5(;&#YT=,x-HP2T9,Gc>#Jih;$=fqV$WeXG)"
        "FQUX8ZVkE$)RXC0#5'N(Oids.pgQT03RYH&^]d8/.99`?9oEb3wW:T%r=^$5KCRHEhK28W7LA`.R,g/3eavK5KUTlud/+]u[dM6TmvBiSwA<`F>MF-=Y-4'0e0kx#NN<rLu17##XiE=$"
        "RdH(#0pm(#G8Gp'7k4/(Y94,2VM+q/+Duu#JksB#AX+=-@)RpFFORF4,uAu.$Io8%?q[5/DOi8.jklRA4.9J+R-#;/j5;.2Ynn8%hNf@#E.^C4>U^:/(G:T@_3A5CX9LNi7r.8P^Jr79"
        "KhdnKEGCV)?*^79'<*XU7QoA>':Y@11g$@7xuP8pn:Co'?ng=Y:-jx,>qWqe8K>S#Y)T-=(LXtM-NOp0]RRM9`QR:0aXAb>gwiM/0nOmu,f'HMb#^Z77cD;$UpA3kc*':%cn7T%dcG>#"
        "%'`_$*M(Z#T`%/L&2>>#78o-#:Y82BJikV-wj.##],l>-fs>n&%0ki0f+kURb3wGZ2/Tr-IO2qiveXI)0-hU<pUxI4DS2e=msC<Ar)4r7g^(5:FtNSHWl[fS^P$(&m;Js&#+XqBsRSm/"
        "pR$l(&N9)Va/>)SSF-##/mYR$fUYL%W24[>Hql7C2I1N([t`87ue5M,G>W-$u#1`NSl(G#k4-2PPLS=#B6]uGgpqV6GK4Z%9J5N)7Yc>#rq:J:%&###JG(o(V,Ea*b@.@#1INh#wC.s$"
        "CUAQ#=+swZ<35V?C'ext$iaS@p3@c;8e/S'`/;W/.>cY#)^O$#rKc'$D2/#,j3B4&Y6o(<NYCs-$$IiLM=Z)4[pi&.g#4jLxnfsXO>W[BXGc_g4h5s[k*/VMQ_?PNeDjE[;$]Cjg5aE["
        "3o)20K91U%1]Cv#cjGU9(u0B#=vi_$1]#gL%E]h(>AJb-T^gQa[HAO91*nC#DwUcM5&-]r2&&VZU+#kOh6S=lt0Oq%J$;cuGkBB#0(CkF5v9kF7-OkF=/Om1(BY>>0/q#$7W(v#<H`2%"
        "UeHa*DOp?>U>g;.pi/^fZoQ>>p=4`<.^@x(L.BiYtkW/1ncM0uS,k1u;*,##sT]F-C5U@/^cCY7>j?PJ6=Z,E1JY##^URp7_H;H*qm3:.RqK0%:RY%eIdnO(1@-M,K_4K&CQ$@K16/hL"
        "KsJfLPmk?%S2ju5KCe>#;e/Q&6iqV$HipguPfhV$TdnI-F:/I%#AEE+d:+.)*lRvTsuId)0pux@p92H*(0ud5XtN4Ye+3]&c9;3Y(-6p%7<Z?5wr*ttg@n$;v.vM#bsBp.>bo>#wllER"
        "x3*##?0RJhb6ai0K$pw#);P>#mD#W-co/L,J/,##e13;gr)'%#<Nkq.tSCl0Z<<P(JjE.3)D:Z&ch6S4eh<MY7Z&VBOob]bkKcm:hJZMu/PE^I:ATFuMF%Y/;MQ%b-aZJLU;dH*tRKj2"
        "?AP>#A4Gc-4#'HQK=a6&#9@D*&jn-)cf-X$+`MjOt-37YG93FQn_/M,1TsUB:w%xKV(iZu'KCaO4)7##&t7T$ZdH(#rvK'#]@nN)Ar6s$(Lt86,,,##%<+?#5l1Z#*GuY#G&%s$9#A-#"
        "DY)g;^1e.=I(ns.u..q/nXJQ0D.C>.R^bq.r`A^+&sA+*KG>c4j@h8.?m:9/>hs>T8e9a#x82T%Lstr-Sg0^#@sQAX0&+.5an%cArW>*Nb8RcEs/(7)vIt,Jc`W>#9LghO&^bQ%U$n5Y"
        "bjXR2T]V^k?^la6b:^:/)=-LOeEF1pq2RO(TwM.[AJ_?TH)>>#EYP1MBH@5/l5$=(c>cJ_K)bC43lr8$5A[o#E#.8##)>>#NG:;$=T=gLXQ7IML3xq$%b7fhTw:Jhc4TY,Gl)5B]G=^,"
        "7a2TJ,OfF4o=]:/Q.)hhpbe]+:m'XO03i6<;54d2*#$bHk/+]u$kqnuR,n'T)$;Qi0@9JL)<.G2xjJ]=[T5s?.f.h)p:k226PC;$>JwT)vT46/kRL@,'l&RJ10oP'=42W6_G.s?Ga><J"
        "PO+P',1(<9PK<22us'^#6fTu>)W2phYrnOA0+tYdgo$8nbGqG*2Vjm*H+G#$2VK4(8@ala[Efs$LaV@%=rDK:M;5H3P@;Z#EU`U%%0Ts-)`lVB2g0^#$XCd%4LO&T4w&T.#nYR$hfA>8"
        "6Vg/)D5m:?uu(>YwHt3OUIE3&`/+euKb''#$(5YY(7CMgRjf,3uq'RJ<'6,E`6`v$=mTB@vAXvK0O9RJZA>)4f=&308E_#$t-aNPx0AW'9isX2WhK`O(UKH4F;)N#LUx6EL:5D#xp>ru"
        "w&iH-qX?T-Lc&_/[]:Y7(=bT%ocZ39l.$?$bKIXJ4A)-*]l@L(?ud)IJ%39/.q^#$(7DZ5nwYYY-)]TB[u_;$Pj>m$@96JV_3BnYOXo9MLw2v#Sq[Ju6(9bCiU^_%g79RD^Pmv$Sd''#"
        "O$^7.1nk)McWT;%es$#ONfp@q@w$$v[v7T$pw)`#iJg2#.GY,MA-ZJ(qOM/)$`b&#;2/vLG/2>$nXI%#qORZ7'/)v#:xcY#lNIkVVh;v#>jZ5Amwg>$apKH%-_o@#sHQc*7x'j(;FH68"
        "E;p,3uu@1(kIX_$ghVxnL@x*3rN7g)DR2/:FKOjs8ba)M5;;te>rZR<-Kxq9__'IQRxsT#pv1.NmtL:vhn8,)sANh):lN'5fIG)uxe)%)aHW-dvi9wp7`WRMdD^1v[CCKE`N3o8a^9>Z"
        "1(G&#IQmcE]Cq8&lv%Y&Sw%s$D,U3JbFU'#IX-gu8;@<$bO^u?@<hB#+V1Z#L1M;$ssNhWT@?>#bwO6Dti1#$5jds.KFFVLqqYV-:eS@#k`QX$mo`T%+87<.YA4I##Lr0$Em9g*PL^8i"
        "d4r><*Xg5F9gO[UnwYN#+9&hu7$x@M`jI&@H7mST#,QvBu7-Z$#c8j%QqC_fg-*##%$lEn46O-vu]<;RvPdA,SC=#-;7Y&#meFR*0dH(#B:r$#aiLY7rbPhL6E;,D'6lW-FSM2i&D*O'"
        "R%T+*lSr0(eV@h(>U/2'<Ar;?aj>g)W<:@.=p=u-]>;,)NcWF3=Vsr?6Qw_O*Iq^ub`3M(%NK/''A-QNS7q_OfsCEO$^')<]XmmY48ii%BfK>H%_-^#@.v>&ok)d3+)Rfrfk`TJ`*B&4"
        "o8F5/ptFR*=TEa*t$FW-DJbp'C$]:..uC5/=jZV[mO*nLB[<MgOo4p/4?fumaODpJ[g#a3cv*j1m#e&#7viuLpmSD$.+>SJ7XA&,-w9=-#A?W'Z`Y##j8VS&/L]EF:MuD4,EwSJ1&gG3"
        "_:@X$Z6j4<*SIs-,a]oDEYe]G7o6<-Zv4Z%LO+mul%*J%*Ex^]t&4_]Sk@a5Eqda$%>`V$;1D;$xK,+#(/>>#S9<h#B)&#,x;,##Ntnh*1pup.=k_j(g8s<^EYl`%;A6gDb^p;.6pg.;"
        "SC7mK9AB[94nF59'8Z<-]#Gv%T]s?R3Q=7%_4i$#r6:X%:*cJ(Wi@g1-DG>#91:GD.S:v#5;$H;&*NP(iV7L(au=Z,>d(B#F>mA#ZG<F3&0fX-8/6c%<AjR#Y,'U#<R&Rd$?UnurJb]u"
        ",FN>fXQ;`sI;Slu$tPjuI&`o%*1vVo0/l-vR#8p$[OY##[V=U%ewbf(SmtiT)^oo.Lj8G;dKO?#X>-C#&&m<.-V:v#5PrZ#rswu#4iUv#OHkx=M76Z$98)c<D$[C#XNGR0O$<)+gaTq%"
        "Pa9B#?R)VT&]oL(xH-oD0K,_&TuUv#nxIT%u;Rv$xtTbQhEo;DCe[AYn-GO:VRcYYo-q=S6`7c.Qw7T>3[AQ2%xGD>rFZqrHVI:QL^vf(m?\?<Cl=pFSZj9a55ViHob<*nDj]1pfH^_d#"
        "%i;we$),##$dT`2q4+&#u1%Z7(5H>#$0@@%P1i&MMJ]0#+V_V$)OvU7(ie&,%p-W$%q(H2K7dl/?chZ-LQ07/$g_l0U:?)%b%NT/LH`vB)+(f)[/*E*i#RT%owvG$KU@YDN1BtAPZa[J"
        "oI`?tnf+dH_D8b6FWM]$L5.'Na'o9Mn*82EZPS6W:%5)/abH89]n%edm-`5T.imJ1I*Pe*&.d,4;JY##q^%##-hSN-0:Bg%5mkGM7-Xj$#E[`*-=pnJN/^p;=c>I7Hax9.0U/Vm9=wwJ"
        "wvI%X6v*Tl(XK>W'Ljcu?oFWM.J>(GQ7.lfE#95/iF(##/4NP&SmtiT*hH9/h`Rp0)AY>#MU+9;eZLtLr@B-#L^B#$mb)-*p&4:'l<HE7Lr29/o;Hc%`EKA=0#ew-lp'^#+2+m1RO/W$"
        "9I<p@DZr3Mb:>l';qS1*Z*%dF#FRfUo3aTB,G[N0n=df#3MHO]n%65MpB2pLcJmwL-JZSJN6h#$5%%<$>nxm8ICZv$[pSZJErJv1Gt(^#8;ETRnOmY#%MZDNGL)`$:`b.)[^5n&nwj8."
        "eU>;-V=.<-8O'D&sxJ$5D+7OMOITqYLwp>ZjBuK;_hUVZ$-8sT1vX00PLv?$+Y8E#Bq<k'O,>>#M05lo@_fD3O(Z@&aq4/(>N$d)I50NaQ@H>#'SFa3,chV$9p-W$BhYF#hXkI)iGVk'"
        "f$O7)>i3T%n52T/b[Rs$&0ZJ(qtF`#]iJ=;62@m<D4D>SMQr&eUdNw9-VdZslSfD3=^P727I4mPK-=C#+8dr$N]/)*+JcXr(5n0#hf68%SlkM9`oW]+v_g/3-]C;$X%8/L)5G>#F8,bR"
        "[H6+#UsLnW<rJfL^gXI)_0YW%(pa]+'UZa$=d$G,-Kb8(YZ->Y&O=o[421[uv<MC61(Fv#Q@?5J]_ppX6KW1PlFplED1Av:,Zt=I#vX]EbmgEXUf4m8/9FM0AE[p'ZAQc%pn%s$?=`Z#"
        "t9RG;CXud<6pv&#2lqr$KY,?#*,>>#;;x-#0fat].n]N-8F:-/YIse+6pY_/c_aWU];r]7_8:T2Fj1t.R,-<.pc]#,$8q8.DZP,%inRs$j?Z#>>/vG*Bv8oU`@hL3@iSpe:=ifQ?5Lh<"
        "tA,6OHd<VKPU@jFNO^Lkd:o^Z>J/_J.2,1O'AHC7X<3&?pT=l<=,u]+s7f8LK<PPC&:@+>n6A>#%COr?D;f>,Q_0V.8k@>#wl2p&j'bT%G0FA#=m2p&mJ+P(7Q`g1pZ'u$hvheFDfUs7"
        "DJaKE')1^#XAi.&w2^FE,^o`th);3#Bsnx-jkB;Z^]w%+Ya>Z.Y?0q%/cL;$u,=;$2a1W-uj:6*>5(]&JG<F3q/LV%v>Kw#q+t11<42w,6@3G]2xI>,qITvWGS,QYPwLa4f)3P*40BuW"
        ":;HoIm,2D*`/_xb7v9/1,LYFrG>7g1'0Dcu,J4M$nXI%#?gW&Bh-/M)#E-r%81Rs$;lcY#4Aln(,uLJ:'sW8*FIEtAIUbF+6r-W$2df>#-,###=Btr$i.*3Ncx<V7QvHS@uc;<%EMOV-"
        "Z<eg<<[?29M2(<-D_[(+]G4;-B-s)+(QMO'C=@W$?be@#a/e)*Bq@.*/po3/#0Tv-qR.uQN-k5A>U^:/5f>QO@t`xqm6x#iaV-l=Td#<[dA4'G(_E91_cJ12KE*C$,/'U#4ro=us%o,T"
        ">5S,4MZkX$S%f)>B_2Eu=epK<a,%%a&5>##rs7T$UeH(#ld0'#9N<48&(UR',V1Z#DfqV$DYCv##D>V79L?v$U.iv%8(W'#2]hV$Lb.='2lUV$FUC,DB=I8%7v[H#FUg(,wP:v#Dl-t$"
        "MiX;-1Xj.;RPWC?fpm83Vp@-#>P)g;q8hTA?vrx+-,%s$cJS_8`<r'8LGi@Q:4W?#Hw,.6Ye86&((S;QDWp`?)8/q/i4$##)IUK2s=ns.bx2T/;4>[-KP-W.+-c9%[/O1)(D6W-D?a@I"
        "+#.&'=2`#$a(8T%YJ>c4.m@d)ro,n?kpw/IeeDY7*E`>#1j6jK%(2@8gJ%8F^95lB:iR-@wYk(NQKl%13,PoE&=D*0fGq^RfphLNr*R6Y^$nW0&uKnu:1&5Y,5Dcu*k+ZCGVDuu9PVSa"
        "V'/s$4pm+M&jc&#dP>J$M_e6/#&###v@H&M1-sF$TF.%#4N/S'I_*9%6f(?#A[%[#E'sxFBX?K)_7k***Hm8&gIHNF-L;8.*R<%h6>kW]v=+OZ;F;,Gk@e^P++W]]O32PE-2o2*MC?>#"
        "ksp7@5-I218t=W-4ZbjFv@Rf;um]u'NK`5Ka7dD4#E^_%$5`#$Z=3o9XGc#-w3%p.spvaG.T@7<s_>?.sdU5:Y)5>/(v5l'ko@u8bj/vJ3u?>:&2wM3C)s5K8I5s.UlwY-(e&-H*p('#"
        "e$aY0Y`x^#].e4ZbD&AZ'G4C=2F&W73nc8:iX$##[jj1#R>H?$R:r$#*ii)dq=Rf;roFG':uBf)_DC@-vhi)dZQZ)4)a0i).uGg)Gv'W-LAG<o%d><./OSxZ(9iY#>OM^gFvC`DbM;9;"
        "&)aD*1i;NO8%&KZ.v$W@i_l+MUaGseK&=s-p%ac)(n_%OY.L/)TOZ5/f_7s$Ft<9%1iUv#;l7[#o=.3Docl&#1Q9+%[;P>#_p-W$9DU8%FMOV-')Rs$TU%kMUq7BPq_;v#w(fw0W>B&O"
        "_;r]7qrO'%1m@L(LnNT%N&ic)@;Dm/#DXI)kTW:.U(^#%s0_C49h8I$'_(Y%jPE.39U84PhF]+>P07i4%Iu-YGWGiVxx?H.dg,2N>v=<:Vs_D39ei_TZPWN]UJkm+<kgFMnd6@Lkb]_7"
        "GU8p9+,Guu>L@)$,6+&#7hn6%ZMY>#s6IG;,cqr$CIL,D/R`a*c=ugLlu*7%[`Y##cgGV%)kZ0)Amps-e+3[G6I#<.4*x9.7c'g;L'Yj0Gmm4fP7-D3roq:?AWt;-[R#G%'&bpuN*P&T"
        "h'0lop`_]b2X,C,tgEM0]tBg<?cCZ#'h&r+G*9QaURS60ZK9Qa'*oJ3m56N'[=0N(]NAI*,K6r%2ZNAB&n%'TeY=(^f1GO+1Po%5GbWq%+,iB#g8Dduw<ZW*dK5QP`>6V?TV,.$+:cRe"
        "$[P`*?0`,2j6iUmwUi=9[^(]X8%l+Dao`MCw`Ze*fnLS.Nu.Q:TFI8%lw;T`gd+t$-S:;$fZeh%EnWp`R4$##X'*l%BnV`+SD[`*l%Oe)`3O9%9]JL(#2f]4/aS+4@M0+*8.KgO+,Q[-"
        "44n8%&E_#$'THv$TXx$.RVh$Pk)6)5ZUrMM$1d:?&8ND#rvUfqr:RoubLVpLh#&9]L?r;M9v0E]PEnQ)h0@xFBp85/%rFPfBfcY#>B4;Hls(##ne:J:RiWGEFCNlLl*;`&3h+=$/1i+W"
        "=rEl'+.(<9uagf$t'w6ONVmJ:_f4&u:*>>#<j_%'T6V50sFw8)4(m<-97=b*X.9K14k=/(i10p7<51s$qWM1C.>h#$c82>5Ai@j09;G>#Dm6+#28WlApGA60xI>V73/5##,u<2CMCPg1"
        "bks]75Yqr$+5>##CbwS%h_H>#^ee@#e8RS#.L`O#S?uR[-@P2B$=P2B?Dfu*+&g,M)b[`*gv$#QoRxY-T(;5KA?r[GxNdF<./oa+lH*209oVV$W2%B',vu&@w0o'0:,Zs.%O4T%TfV_Z"
        ")&n/(ml:^#483F3Zp^Z-&9q]=8F]r*,d4K1$+-/[XqT.[B'^+D&PiF<;S'Oh]RUS.?leLpF2Y=l^u<RQH`*(*e0_Ee)/oEetS+I-37Xc2OP;2K:)ur$l1<3$'8P>#d($a$efY>#G(jg;"
        "%&###aCse+QWc'&#;b(N8OL^#llBW&jlOw$L;@L(sP:l0wDN/Mx&_uc;8Mmu=-#fZC0Q2B-=a`a<&V*@]QZ`*Ma:>-RC^$.BVfvLeRrhL#S?##5f1$#gE5@0NY(v#-cLv#2R)F.gD''%"
        "N@ml/)p_V$6m.-#XJZ?$%`aA,hl5R*Pi$W$>Z']0np-kX(@FT%+/###I-BQ&A@[8%$nqv%dsOT%n?A8%:2jc)=N8A$A9`R[3)wOAkOA+MtP`<VeNUtVV6HUW1e3R%rh?g#]q<w7RskA#"
        "h/m<-vLZP3R@?>#Gu`w'ZfWEeFuYi9R+(-MgSQ##r9ZA%f(vu#2g.-#G(jg;vQq&#O&_B#K:7j(*iO&#wasPa0i7$$=kn^(^6+=$r(rB#q.8kXRS,lLRZbA#-#%A4XuuUH$81;$C>Nr6"
        "l$88Ia^&,Rc40#,mm>S.s5W=HQ+:#vRA$(#GqXA[RF?##?(m<-a2SF/L_R%#CD&%8GaV]-34NP&;7n8%ELd_%/SY>#.](?#Gv_V$%LSJ.R2$_#[2.H.F54Z$e.6*4cks]7pZ]%#ua;)+"
        ":@Hc*`tr$#+KdHd_aUL((dUl0b4+i(QII8%b$g:/%3<l(Smut$iL0+*?V&C4>U^:/nV.+rWd.`pipDWuT%E'T_pJ(TthbpUt^H$tnv6(s#'qe%0J,##3-#n&P8)8$9;`A`K$]%uCCH-f"
        "1,4ktd3T=H%Y%a+6d*gLcmAZ7]v#R&+A>##'=0K16AcY#Cxig;.S:v#]twjk3al'&)jQmMKR$m8wF'L5;Ste)`Dj&+Y8Mh1kli]$KG*F35$Qe$vJg[#=c5##fN+_[`QllYD.%^s3.TKL"
        "$@2s$j*;,[,@^*u)Le+Sp>j[tL/h(*KM'##YACpL8nJ`$e4=&#Psdu$+_BIQmCv>#2M82BW3xf1b*Z^#5;ba$9CSv$a.d>#+(2t-iR80O>rJfL^/?0)U=>c*qSVk'EX*9%cI#v,tD(l0"
        "Y/ZJ(vL0f)(OT:/k#b$#DTHJ(LYcG3T&Up7[*e4(v8FPVvJ;gux[C6N_]cln?^U@KG)5<K9VkrmLK`IhAWCq6t]=E#dL%&=$L@oRYgZquj$,YL_9DNpvrQ6WRtes-PRNjLuT$##bZ<hu"
        "g'>uujYB%M@YLY$L`2d*5h%Y-]IOR*LS8R*=+GR*_?ZR**ILR*MuJ<qmfNR*bHZR*QjMR*5qT4ol`IR*(H[R*j]NR*n,Ow9a@KR*Y1^R*qpHR*5e:R*O62^#;g`=-P[Si+g7LkLQv^*%"
        "k%5@MFIo8%k*JfLF:-##@fGD/V(A-#R4RV8sIK/)st*9.G(jg;S<N&O&*l,%sdx(<UJ=L(P_wS%hDU]=w0E%$b9<lZfKs`#'Zvc$>4(7#Dn.nLRGfqLZx2$#15QT1C;G>#);>>#Xrg>#"
        "/oME,Ywv>#[VvZ$<8.H+_5bt(tvN+.Du__?PJ>']v1@T.=M-[-`01pMa?3v#I/_K$i%;xku);cM&kphp'qBx4M0vxu-[Zdb)Il(N5d_s-,NrcuCRI`$W;r$#Ec8U^8;P>#jA%4;TVWC?"
        "&#ZY#_YhpLEwKh;*GY>#3?G>#*ITfLFS&%#+x(t-iIsjN@CYcMf1a`+q]0X:F#nZ$ouqB#-8`^#*l0C4[7,i(TL.s$?BAO9</L^#%?FeA_D2T%C<>qO].-/[ZtT.[)q0J#w'2R#:e_T#"
        "$$)muwsAnux=P[u-29T96377p&TaV-$),##fC@D$,'A-#HR)V7(SM*/o2G>#a5&b[$ex(<nY%1(oD(l0crxF4.S39/1S5kOeA;a#J1;W-gm?d)%f^F*#DXI)B7b):=%-=I8x`0@bW4._"
        "g;#]u7bW)C3vfHZ`.u=>k=%3<J)J51@#*ZXVIYWEShaORsofQ3+e3mE>cNb.%J:;$)$K_/4Sj9^7J:;$e#CQ/'/###:(l+DWPPQ80')d*-'rA#8jtc30Jor^xV7L(nVJ^+Fd19/F*x9."
        "nd1g%U7K,3uPurH)<v`u@S$d2n,TM#umN52=C=0(qnO2B[(Si)1TH*MRpNuLwQFjL0s)$#bUda$erYY#(GuY#7gA>#^$3N&c+d>#ZF')NB'Kn1Ujk&#HNLK(Sh*T%4vlA#JmCD38x*f2"
        "m+mN#ZAh@.Xk4ou54b/.Cd%IEpXiGuE(T4S$,>>#PWkEIf7nEID;bo7*xsp%/Gc>#[)JgLdkq).]L')NTWf<$F`3dX;;r]7@*)m$%dRh($N^H3*+fC#tIdP/bkOH6KcLl#q7o,^-r(_="
        "Z'(#Y$@`k#&ENP$^bB:upFau.C/Iou*J4_JrZZ`*-X?T'/AY>#:DP##=l$W$G&A3%Fv3/ku?kg;*&###8`1l%r/>)<(D(f->eCB#19G>#NcBf-Sc'<&:_Lj(^Kt9%?S-[-H6N1)YwH>#"
        "W/0gkS/=,4ladcZ##vIqU-r?#x^g<MFCWs79.67Kwm%Hu$,RKuQarku#$(<26>TC+>eB>#oZ%##KoQ8I_lQlJj8V,2Mq<9%(/>>#1#dY#+nw8'3ukA#EF5w$.d#cEhLM^Q_Ohp$QL`?@"
        "fr3$%=(%<-CZAc$[otZII/HP0$Mq`?$_=I)j8Na*?tos-@6;hLK5s1)JjE.3*]8/L2Wgn/Z`Q=l?JNia'e8q5QNoD4%I)8$Aw=DuUu%t#0U5Fu4R1&NA5Mvuo&3JMB`Oh#;T0%MO]x8%"
        "[G>+#S1b(NZ=pfh?tZv$qs:gL),0s&O+b_*-Xoh*Ha0<-bECMsx7Oa*(vfk1iDnZ#@'/WVgaL>u58S<;Vm-C#N_blnWS>N#p>]tux6?xL^/F'M((p`$7NY##/]Rq.N$n,v_N]_kv$nl1"
        "uAlY#5dxu#Ks4)<Pl/f)O_es$_1B>,ont@R2l*j$xiWI)>4wkC'rcW.KJ=l(HiiYNu/?j4U(0kXO7KY%G),##nsIr3&V_/#_JuX7s=)v#QaB6&-MuY#6Oq/?`7@kX>pD?8$07kX87lgL"
        "`=>A#<OxF/i>3I)G+8d<U:2N9>U^:/?;Rv$irxF4%VH8%Innjusg1,e5%uUS:BaqF[J26<q9L%@aQ_fuE,:In7nlh?FZ1R9,]9)hBwUb4q0;*#eu&58QYs`#i/78#:kj9%4?LJucw/0#"
        "5IM>#dq4b[^O,GMi&p$MeInxM;/.PJ($N50GLfGNUvA_/0aB_/51-aN>&.>5oMN^#kL/>5ADO?[XoJnNIvbhuS;r.N%:NiM1M,;-#srfD)Q2aO/fZf1DtC^#:IY-Ng1;]PHsXLu%##?-"
        "H-Yv-RRt<QKr@QMZ-hkN>31iulvx>-LQx>-e-:W.UUftuk@kf-I<w-QMN8F.cEaF.*)1>#LUP&#/p(N:<^AsRlW%##egkt-`Pl1OWSfcrvMn&#Hvhnuoig;-810_-+QrR*]o@=mO/pGN"
        "kFO2Bi0XJMxK:@-2_`=-aY`=-N9;X-*[CS*b=9R*_P8S*=6NJM_@]qLaQY8PTPBcNbWc8P4;%iNc^l8P?r@QMHLoqL>eu8P@wCHMNqOrLej(9PA'MHMP'crLfp19PT@7&MTYLY$#U72U"
        ":0b8.C91>5R4Z58MXkxuO.Yv-Y;'5QDRL%OTZqfD'>nw9pR?f:qFYw9<6OW-bfBG.0#hG.G]CG.9r5cD(4hl^VmQGEs9u]PJ&KG.@Hh>;7a/sR]%b3`:e&<;(6^VIU&Os%,YDk=*t6?%"
        "ojN/Q-fr7@,<ZDOI#KG.RclLMXwTkNkd41ugR-h-dk?G.0#hG.H_@G.:$Hc))<$mBVmQGEs9u]PL.QG.A*8F.a?aF.o61.qb91WS'kDB%rxEcMc^ZlNK,c1u?)>Z-RowKj<SUMuW$@b)"
        "&5*cV:U1'#UxfI$n.emMM(88%1JNfLp[_VIcw3/Mtssrnp3(S*=8oR*&S9S*RclLMF@]qL@Vb;PsZSfN)]k;P6G7iN*ct;PB.]QMHLoqL[i'<P545GMNqOrL+fkvOY+?:vR;+#H#Qu1q"
        "cRF3k0X`o$Gsi'&W:w8%T@>I8]>L#v%@uu#h>QtL+$:W.qn?4vNog%%m`5W-1c>F.X'aF.L)rLKP0x<-V.Yv-Y1wFQpi=gPI#cLu2wx>-vRx>-Mg]F-)aJs(iJ9F.a?aF.><)D*C]-X-"
        "EG9F.cEaF.BZ%A+LXlxuooSg'`Dp_jV.wt-Yt,AM*$`4fRZIv$7sq:9Q5PR*EQrR*s@.Q/q;U4oX2@,M._ADX4cq;-VXRj':%9SMtoJnNFgFLucWRj'OPPLMgv)mNFd41u^(A$(_t8`P"
        "0ON+R`^VePI#cLuFOkB-r-Yv-#2g%Q`Bf(<`X8n/2^60##,Y:vr:Au-JraZMP7I3#1&4a$gB3+0x->>#ejELu1On/(&)(F.Pr]GM?ZuxYElL>?s=v<-iQx>-ZqXv-))LM8;_4Au;Tx>-"
        "]qXv-N<eNQ.o>WMc^ZlNK,c1uru#2(G3-4i<SUMu,nqa)mV3V6:)0rgESfcrwSw&#NSEl$:q`=-Fv%Y-8*GR*)e*$M:gMY$hnBwpK3[Ih2%w<-e66T/VO*1#QRxP#l_d;-mA01(B]cw9"
        "@/afL4=K$.0Bc9Ql+w=5,B)&Pdd6,E2g[d+=bdi90R>8J/Zi]P.iMY$14O;71j7#Q@nv>@;`o772sR>QZUHG.92KI=l[WpBiZR78^q''#awfI$oO1?%[988%3]/GM#)P:vw;MR*IiC5/"
        "/4$293'PR*Qc8R*#?[GMLl`D+g3kfMKtP]u7gOkLT8t]+StP]uqk`=-Xw`=-Ml`=-Yw`=-^l`=-Zw`=-xk`=-[w`=-wk`=-x';u6EK=VHO;sS7tIx%+]<m+DOYXm';uqV$x6@G;9&no@"
        "/C+f*0S:v#5&cV$V3Xc2#a_;$2RJY>5hn&,P[ii06FoY-+9ai,]qRj)Yx,##O)6S/6`oh(Egv%+<Q=d;)eD1)-g&+*(@n5/PXYQ'ml:^#qlT49$HBa<lFS_#Nm_nW(e<<D[AiT/mjF<'"
        "%HTrKT&hP?D1pbHBEdhDdARf;Rr&%QhR6CubC#h0Olw%'C]r1=c<vl&*id3u8xpKfXWO8^mFP)95dQY50hFmuuX(_$kaR%#,iwZ7t](v#D4dY#hOSfLa3jk).oCJ:*Juu#@LhcDO^ka%"
        "t;tR8(4F70tj.##oxQW.J*a`+^,[`*9l&Fe2DXI)N8Ea*DVWs-:o4gL-SdJ(*)TF4+87<.b:nb%HC<^#SKZTFC3Ir9Kj$UK:*M:N7]OkCbwMx_hG[eDh)(VDDS9U>(?5IP[Rf/1^:5SM"
        "^u.eu=%<>lx]l4Ak`sBR'2r[Lq^5KG$),##%j9j%$,B&4<=rr$3@>>#C&JD*G)wD*XmVa4t7?5/TF,i(TT.8[H18r0]k&r2)vjSAPi*&Z/omx/*;H&uB2dYNaIMuG-lBB#(*b]6odxtu"
        "Bp`Y#?OS4o<@))j#KYc2rF0N2(,,##JV1v#-0>RCu=7Q0wP:v#E1Jj0%S*R0^tes$m0VS.;5;q.#VB+*V%M=Q^q(9/ND#G4;OBZ$qD9p7HvUv>2*&6EH^fu8IP05DE9_iNtAuXg<+<9G"
        "&pxiEfwZr3MQu5O]oWfN%0oD5*58N:ZG#g3*7:+efEO8QFpN(Jl,Z&5n5dYJVIj,8SMRlcP^;&S6*,##/?DK1MUx%-KcO;-B^-d)SCI*dQv]G;(2###;+l+D.cCv#Md[m-YS$kk$D0X."
        "ixHW.ncf#,5(Dd*QC`,DUhHhcf*2+'-MbI3qP9WN(pr-Hv+wP;UC='fiN^NF;#YOHaQkF>(1*xIP*(4a^4$D?Ze@g3o(6P;h*H&JsRIg>o-W/3jN#X.g3/b>+_AG2VRTqu?<QZ$e`''#"
        "(IE<-e:)*/=u@[#])14r-n(-M'Ggd#PRMM0xI>V79u19.&<lu>$ctA#dNf'&D7I@@-Fr9/;Shv-pD(l0A7Z##V6t(&h?>G,F(Vo(k/0[#ML0A$uwtpu`GpGO1?V8<Qg1'?xB9->WO%EQ"
        "TmLv+,r=^u-$B>c/;W=l%*/'IK?X;7C#tFZrdxQBBux60/*qhX=j`S%PGnc&ApiUMcNViult%T$ske%#4iPI)=TZ`*6>P>#2;U@#wDk;$%2Abe-DJfLn@>*$@X7m8ZAtM(CjO?-%RV.2"
        "bks]7Ab+/(F%v##]<'>$&Ca8.bp1K(=nOD&ja^:/XkC+*StF`#a?g1YliU(6VoU%YZ?Q$(I:Lga<o).RSqR=$&cp*7'Y3BuU1a=EFcw_+jnS`?;P-,-1l()jFa_c)&@(,-*I'h;.Yl>#"
        "@)8a3CV,##/lK+*mr<e)Vtes$@'TM'pOME/]6f[#qmC#?tg.I*lfkV-[qQX-WQD.3%*Uv$2d%gdScpiR=K?Z#c/o3O4SxDNf_ke$mn'fuQqIxXJqru>M-]]=iq0uu$HE;-'<(V?FbYCs"
        ".A###LnZ,v)$F=$Q4i$#h$:9&17NP&+JlY#1Ee.//KA>#*c.iNBM-<.BV?W-VHHCbf%'E#`2.X)wh#n;V&wN1W5%bPE<QiPBX/x7,>39)MPfuLAJZY#j[(REA7'a3CW_c)[1#9'7JP>#"
        "/5J'.e>058/l0^#<4PG-jiV]$6nBK1Ku__?1O,n&fiwd)b?JjMK@H>#,QTs-cU87JD)b)<nFdiP2?85/<q/a5.HI`u`U`/)X+l=$vAxFDpV>?7pGLB=jIoP#LD#4@D=0>YCa],#0p[)v"
        "r:Oa$lB%%#CJ;u$uqUf:cvY3'Ht7nL2vEa%:M[S%D#DD3][_a4mV3i($@n5/P=QP/ko:*,/Na.,'b5B/Hp8dZ+f5auB-c>,ag^R(Xk2]fc-vju3LoGONLwx=;=3#v)Lolu-TLV$N.Rs$"
        "$('d5dIa)#rNcB$drC$#oAQ6&2JG##?]O&17)ddA8Vio]H;BvHH_HgL0=E$#Pvm)*B#@EP[)CB#l,=ci*:<CI3?lnL_$Y`<fjFHPw'_Y,&k$^?FBtIUR?wE/&+g-6T),##/3PB$RV9:#"
        "(dCY75PW-?[GcY#X**v#?n=C-;%VJ:RlIfLLIT<9DX-C#.lFJ(T3kT%bx2D#:ql=(_0_=%U2_8%'Hrc)2[1$%r32]/iB7f3tkPl1p:gF4>uOVF)*g:9tY?0Pq28b)7qQw9o'>FI5/lZU"
        "+#_R;Ta.4Q.q[KFB?n=9ES.)j*tj91+HrTC@Sm<JIONp;oK+tA*`&&:&ii6;(/7W.DrjNG,Q,52CT>NDpxep<<o*]>M#78'n>mS@]S[`*p$Zp'<J:;$@SlY##3@@%<Ro;%ixY>#)G:;$"
        "7^C;$Jv6+#^42W-3-]'/p?2P+&Aq(<u3F$#;_R%#0%h'+]P/B+27E:.v_^F*VZv)44*x9.KCI8%icMD3Xk4]1g*l%A'Q)>2wiH8/(6Sj<$dXLuBj#t&ni8BJ&k@xFgO-v#qlq?'d.%O="
        "V=61`B*H&+i;LM'#8/;6[KB@#2i:?#u,WK;;VP>#uJcY#6Ewa$q27+#<bbZ.(xbI)u[^'+Xx(W-B8H+<kFiT/]/r0(ulQv$[-5[$m#B[#C=o2@,]lVEW98pPaw7wL8_H_u?EwnR/qvfu"
        "bk/>c;w4/(U@vou<h.6d,EQW/mR]out-eiLgBv&#Z(a#$&/>>#KhX=$q3g*#;U=6B&&###O[YF#H<eE.rvp#$`bNk9#1P3)^oTH4/v@6G[jN]7-X[%Rd,]XLQPGXS,lIGGGQPtK1kqjk"
        "uhh[6?bI?\?/2A'Ij%8N:5rnXTZoF#?Nw?12LxTA]l*?KKKgi%%5vP20Trou,sp0[[,lj]7mxRfL@Hca$333m06*^T.F>-&#(_Sj-vZ3I$5(1EQbfr]7T28>,xS)o&6JG##6:Zq7D*$?$"
        "E?dsI1n(9/>;gF4lCg^$IX:f3CGRv$%`dhG-?QjsD&g`ia2%(IbeX7Nl.K%7c%7F$:LD>Pp(KcTN^@+iB^K:W,u)rm_kr(hQ8a=7+.>>#[MQ4oA2f>,sV6A4XK^@#ET,r%0+35&*J(v#"
        "^@/q`t.HW-McT-4RF$##TG[`*&gg88^<&'HoX9N(vBo8%?/'J3@T,PJVGlW@/@J#QSSg.@CGNf+`2tD$>'b:Hn1%6/o]eE*VrChLHG-##5v7T$u]Y%0F-sV7JSuY#U71p$(ZhpL6,D'#"
        "%YW/1u?W:.jQ+T%)s.)*R%Wh$.d8T_Wl'L3LA^+4.0-$$vU>`O=aeBg8=&t9;hF%,s;+hufXudoDR1m#LQB(G-3BvV5hGmSi1BP8Ax5&FuR=A+=0Wf:N%dG*EX*9%5Gd9%ioc>#3As1B"
        ")85##O/,)<;Qt$#'dtq)x14GMVfr]7m#5A#*Qo&,jb`$#G*Jo8]F8m(TeRs$Sp1K(.W8f3P3Cx$eL0+*a#fF4i9Ss$^@[X$CT/p.`0bug_&^Fht7h45A#^<MXq/s&AgKB$G'ssu6,l2J"
        "qIo6*wQ*[u.Lg]+5pL:v+-H^)?XvinRuUH@n(Bnf/6T7#[qq;?:SOeu#VlB$g:r$#;w%$#-rrB#[he@#Cl?8%MpKB#'wKB#NPO1FvEr$YF.alJ6i$ouas9GM-fw]@%EYX7;4_k#G89iB"
        "MvB^#B:rh+qfVicUn$)#rhx?R)%:>,Kb*E,sPr0(9JY##sAP>#QR`i0px-_J0,9>,fwwS%J#j?Bd?7f3@>K.*aBf[#E$3j1=?HT%,mW+JT43D+1g*fif0KQb8VR>TM.kLB_vqII5[9tQ"
        "BOuIUYtw&*^tV@#?f6cZ$n5NuVZ[X(HFOfLbMx$v;<EH$m,_'#1c%[-'>ffLwW&%%.GlY#K-f>#6M8MB1J0gL]W`'#_Eng)&[=T%O(sY$:0Tv-FcR5J#[Tj(H$0[#>Mm8.PJ5c4KX+T%"
        "[U^:/5#J)*g:s&T.twE*/p7HR@cDd.W0a78J7Wg*`w5&@f,ms.R%^duS%krur/jiF>i1Jqfo*mBfBBdAFa.WB/BtC*kYm5bK8v>]S+Buu,@Tk$J$)c#XS#lL>Go8%S'h*%=_-Z$wv%Y-"
        "S30S*L/,dNio/7NVLI&M4)xIMPe8jMQkAjMs<VIM'c`=-3TV=-4>;QM6h&kMO6)o*UHggLS[W.hLF@)4K^]SJll.p8af0ZHO;sS7adI21G#1j(8M1_AF(s92#3IR*fQQR*A;SR*1K6V?"
        "UxNR*hWQR*a(i*%T),##XI:;$soZ(#u/CV]i?Bt8C2=T'<[1B#8xt+D))Rj0;8P>#Vc'##'9cu>Z2;T%PI<D<TG=L([>j&+&sA+*.=6g)4o0N(P5MB#2-._#v1m3Y;Z?&4Hee.77:?_#"
        "Deh'9aRps.D=2#G6x:kFob*E,q/M=/,2QT%g8%O;-t''#l9L@./Ff0>9sKR/9%x[-U-':%b5Qv$bHib=AJ_fuL0)ipo:soIT=s#Ph`C%*jp]S%2WAGaHZF)4?NPh)[+^s&L1Vv#s-@G;"
        "8C5h%<gAqDk4/h;wr?<]/t1-MW7=PA8OL^#llBW&k/qh)_%]L(sP:l0V$nO(Ei(?#Mn_Z[<wCe3sq4D#I],q%-Er(1b31&AG7)Zu+.+]uN^L_:pebbrLEnkKBn1I6qP6-u&Ll-41wk2v"
        "f=(V$rqn%#5dxO(9wF/([gJe$I/>>#p-[5Vuggh;$UXm/FXYF#YSMO'$TH[$j;Na*QS:b*vElT%jbA2's8Xk;c,sCQV+if?2CkC?IA/Wi]K)]*(_[@/^=k&iON<HEmvAOtBb`HLQu*GM"
        "<Y=p%-w@5/`rmYfQcDV?4N^JsHj0W-PL]MsAK&$MC,;lMS^f7RX02'#c>^iuuwQiL`#)hLGL6X7fX*9%#%MJ:V9-5.QD[c;Ma?C#>l%@#`x@1(kNo%#2FV39gDHTKu.`^#Qe>x>q6Ld2"
        "D.QRmJWsC^RdpiRI''oNmqkI$'Z[WtHLaq,a'W@#Paur-+-vwOs88Hg^<JdgjV1nW6ejhueb`S$xe0'#Fd4%#-rrB#VDx@%T>>1:T-C#$5gCdDH[3v#]Y?mVu^5;;90*%B@Jl;VsB$(7"
        "'XW=S`j7N[Ws*A$MmIuK&`:x5%j:*HS^B884VrwS_71$?M_gu.RBTmCYf)6c6.Ok:vY&]tDH`>,Y8`c)Ya>Z.<Auu#&Z9W$o0g*#UENh#Va]Y#1>6X$f<L@.hdGpq-IuM(P#fF4h>c;-"
        "Z,=b$h*Nh-iJlL^rcoO#WQDf:,S08@F:tB#h@D?u>iSJLFvEo<'ZR&*.)c00ku7T$b*$;#*63U-#]Z*2Y<it6,(tO'TC-p.fmsf:Ik<c4ZWZ*bg.fV%pc-#6]XhfCX'Jv3ix[['D,j]4"
        "GCAqDt[b(,vRkI?7hnSO/I:FQ'3$59,%uA#M3j9D3cj7@qLY*kqDU`3IMT+*@Sc>#,aAU'm+3N05CcY#LpB'#rdFZM[Tv]8g6+++fLXI)pD(l0Nd19/07I##:).T%.-Ol(br.$$?mLT/"
        "d6.YumfkI6Xm/>G,^Zg)i]Hg)eJkf1'*'G;A1a,Yrnehu+#4]uT`1ifXF7K#tq1<MT+m+`gfR7IOv>sJ_w&a33^^f1[mYv.B/>>#ombt.xYr0(>]l>#P'AnLlkt1BPe@J1_Rm>-1i]fL"
        "R`wP/x/ug$/ZVO'EdHD*v_^F*5K_w'H(39/#RU&$oIP0d08=AHI=#n&5nQo[='CLV2FJ%/Og&N2ZjV41x&uU%Frs`#X#OK*0^5sM:f_MPe@e`Exi@)=G>GLSv&.GE>PXH=v>k`@;lhMP"
        "UBS3O_q?##Yu7T$kU9:#cWt&#1V[Z78oMZ#QpKB#%CIG;)SUV$AO$dD1S:v#6vx>#Ln9'>VSb',sH9D3qD,b+GX$0)[.md$%ej;-/UIq$6jE.39I)B4KJ=l(t/>u-nP;^4N29f3p`=Z,"
        "AX;7B%Kn8@ch@=KVOGK?kH7^R2^[4Q;Ru%@n`:5;b<tT79)q`uV<+#5KY:3OCF+E6gn.@<uWoYScjD?>uZ58iBg987T72,2IxT-HBB,D=0hQ#7+Mu1hrVPQP<[6D-]iA^#',VEn4GREn"
        ";XEM0q02)#,S1v#XmoE%0`u>#/`Lv#EGET%7m=;$[EZl.3As1B+FOW5FXYF#D7<9/I*&&,+G4i(K[ns$JHkA#H<R<&i7tv%sp7f3o]d8/EI7lLaRjt(P#G:.KiP>#_g`I3q:Rv$,9NVT"
        "1:/K=ock`l5>q,YgH)#+Mm]ruLc_1k^x=gF?VDNG8j5fU:YeL#S8c>#$f^pUNH@WtsM6`Epa82T;%p.UE6>##-[eD$U8b9#Ne[%#rORZ73RHG*>5,##-vt$##3ew%xnCJ:8lfc<wNBw#"
        "]`Fm0aFeU10t1-MYIt1B@'.C#A90w)`?+=$r(rB#Rl:a%nx2T%mKgw#v4oL(i)-a3c='C4,3^CgS2/=WX.C1bOoG9MrBZfU)Ac<L>Er48LwmukeuT3&J:V@#N+e`*a2BJK)xi4uHO`J&"
        "]QCWdVMiJ&Cn'wc+_AG267.wu`k7L$]ww%#uDlX7Ci[^0D/fO(.JcY#Q;ofLA9=PAsVfm0(m_V$Tv-kX7S;20XV]g)P5Zs.x5GT%Q#LR/c2v[-2.C+*4X2nUlu=c4DELG)Et/T.[7Rs$"
        "&CoT%R;T+4t;cY-KNS*$A8FK2,=:<Kv9Ka@B3[t.1FhiB?87D67kZ=P5Qec=Q&8+W3^p^&fkJ*@g%=d:idJA$'f$o?lln77>)IAV93GK#,@u7VJ5V-@:lI0Ym)6PJ+wAe3$+qu,H4-<*"
        "2.'o0dm==-/:,L%=:$)1d61&AJ-'q%JW.mc08O]HwNwP8_j8H<s6nH-&&/HXTUZY#/2$A=2l:kF4lDM0Ya>Z.H25##FmMU0H'8nL-K?##r8C['[s@-#YYQC#=;3X.`0j;-ASDu$?;Rv$"
        "&q[P/lSYN(UQD.3.+'7J`wPg)%f^F*PVe,;%4DoU//9$KskPfUlu_#Ax<I-gxRhLDbqi(Qp7;gGI8a@#Dicvh(:S6l4'8>-QI&SS4k:?#9oCZupGoNaKQ/6:CiI81+h+%7lB,8&bpP3D"
        "YwvY#:85##,>G>#u_l'?Bl8g2L*B+Eb#`50qbWXmAxVW%gHq,36FDM<cG&J3+&+iEYR%h*-I$e*0C@b*@VK^4Xh$##BC4VdFN`>,aM5s$Zv*##L:dl/>RS9AVscf(.n$b'N*a]F0:ZK)"
        "(>YY#Si%/L8?M&#ocBaN^FZY#4ku-&+cbL(RbwS%9o+.)9YiS%SLI5/A=M8.Gnn8%Y/#0lRoOV>U]o8%1Hkwh$FpauCkC7u#2^]td8BE<:pgeqCn'bZ/b@-#V)uoL6/$Z?q.&)*11(58"
        "j86A#?=i?#%IIG;P7M;$q2,##->G##,b;(Or,Tg2)8w8%[nV*%N[%],:_2d*%gRh(/?Uq%4KsH*ItaV$wZ'u$@f)T/2K4L#P`i=.9v]$&8XOti[J4N@+]lVE$IaCacGri9k])8Ua@]h#"
        "wTu>#RV>D%@6EOO>>li^bvFW-T(Ps0HZS,;DN8,;*25dN3#####hPDOou`=-8ZU[-vs=T*ZsG*SZ,x1,$]3v6Mrj]P+w`=-Ev%Y-Da9S*FTrR*:3QtO^1?dN1Y6Dgv7r]5=Z6;[CkZ;%"
        "&5poSQ>-g)B2jo]A_-Z$Vm`=-IA=r.CooS7OmmR*vTq5NU]`=-JhU).hM8vL+@o8%;R(#v'BefP0$tJQdU[F%S9.aNltu=Pv9JY-6irl+t:v8.Km?1c002&PBlJY-xwnl+*`v8.$YBxp"
        "YGZ2V%]<<%LGvl+2xv8.fHSri13p>I$Y<<%&s`C<L++kkMUg+M_v.%B4uq&Oia<:i%]n>6)[ccMi-6V)($2&P4H&Y-k,5S*Q@2x9G0#dO^1?dN*nfM-v7r]5=Z6;[#HM4)Mj(V<:socN"
        ";$.U&pgPDOEv`=-Yb&kMFR_Xq($2&PgG&Y-Gn3S*Q@2x9$^9`ODGhsNw*r1PFS$tNGx;,%Q;5wp^m(vZGK;.M%e`=-A#a=-p6kB-*)&Y-OgZR*4ckGMa3%##eujo7l8'x9]PuSR#7SL,"
        "8;98S`7Up9$Y<<%l)K`Q.eO;.cD)##0cR>65nq8/EWkdUfLB+e9EMAPa<+s7Pel8/hOi:Rp0OG%W#BvPZa+WS%]<<%7=fG7R&Ys-kE`'8HBi5K%_pG)TBF]=UtocNbPY@&qgPDO)v`=-"
        "=%Sb%;g<0SAOH+[)-MAPit`=-R@0_-T>:S*V'p-Qe]_[J/FZ=Mtm%Y-Y-r:28J1x9qUYr0:c/Z5=Z6;[CkZ;%&5poSQ>-g)B2jo]A_-Z$Vm`=-Ag%Y-6kL4`G7l5p5_fi'laAJ1N/t2)"
        "',,##Toq;$X1[s$w#]>#f2i3;I5WC?5SZ>#Eef5B9uhv#Rl8a37pXV$Kv6+#Bx]2BZ*j20)J>V7g5H50-Y_V$8J4.#5,YG.[QKd+M@i;$iV_v-D*&&,$^;g1J35j'I*XT%#W2,)m44a*"
        "b2@<-P1dW-Q?#pq8w>)o`HSX#U#9Z7Sbj>7=p4I$9]6=7oCm#&P&72pB)6gs2DVk1hYpV-pB5E-qHSqR4=wP/?w(+*Eax2)<7[xktmo-:TUps$C@d]2TFVkL3M+48%p9jC]P]q7M8bp'"
        "Nnd/rUuou,6xJV6e#=N)K(2v#6xCZ#[u)D#n:3D#3_[G;(a)b<CiA(#HLLd2::UcD3(V?#+D>##Dm_V$GI@w#9+i;$GGET%?>qo%Ov6+#NF,3BcN8308Xj.;Ms'L5A)SM5Ydr>$MOY'8"
        "m=&h;nYk5GTpo`?w,c>-Mf,##S#O.3/Ll31f./m0N*n(+5n.x,WEK6&C@E0)NT0U%24vr-E@Xm/`[C+*,,6J*6Rrt-EI7lL*rXI)=Vd8/-fbA#]qFU%,<;i;2itJ22EK,E0-Q[7smK.E"
        ",u(22S?DO1h)P]FOAxP1'bl`u^qR?(v-QL2SwFo9?N=@?pqX:dODL[6O>:[6'PO]u8aqa'$Nh5MJ^S,;3`jY5).e;-]C?a$ptS4o;6)S*hSqpOugffNHe?:r($2&PjG&Y-It3S*Q@2x9"
        "'pT`O^1?dNX^.i/iZSfN7</#MIrJfLmIvmusqdIMWMjiM'>TnLH3a=-o0g(%m;O9ih:*:2Jgk;-]j09Q7gv&No0nlL,B-INA&SFNA&SFN%e`=-d0^KN%e`=-Ev%Y-=O=Mac+?/_uGI>Q"
        "Ct`=-)@7r7l2kw9s,nR*f_1dM?'.aNv[^vL'N?T%=-WbNEVQpO)b>;dEMfl^57Y&#Isjw97Jr20e:5a728G>#Uj<Wo_`G59Hui8&ZQ@Zn[eB2B`#D0%:QAZnh_F,3MH9&,f9=X$=SC(,"
        ",oAq/+#Z59ClrS&x.YV@QS:B#_6]auTq-q$Iej>7=p4I$:Y-=7NkU(Ao`Tppc++PALR*r7Nsx8p$I$7Aj]_^#,eUCsEt.9'o^5VdWfk22o8:Mq$4a39)G>N)]1$##7Rn29<s4T8O_cs&"
        "<):<-iQ<7%vTZj`&*pk&bOiZ#4,m5/?;US%L?n29qb&g2$^3NrqO7A-MX2%/A8Tp0]oq5ql[)U8at@&6NsN.3;JQ=-DHO*2*K6aPs4H<4OP`v-N%:(.[Q.M;rOJgs,Lj>7Shj>7G*]$("
        "H7N>-Hx+=*vJu_F=Mdv$D*$$$TTX298tTMsdgN49++PR*7bO:2d3AvP@8m9%t<0q'<b`=-w0Y99++PR*DNrR*Y.1dMFS$tN@8m9%Z$6s.m<0q'<b`=-BeN49++PR*?=lR*O(n5/8wfI$"
        "FFj+NmNx8%juC'87o=R*n;'x9`PuSRT4b(NjjS>-Y@+s7fdl8/0mZ>R,RoY-n2Z/-S/u8.%l>:r8<2&P$;pY-YAV/-lxu8.RKai,QnQ&Q%]<<%-bT/-&Sv8.>;rc%1L@dF$Y<<%joK5'"
        "2bahu+xT#v93n8%R'>uu>Hb30bG.;62oH;H);d'&TXQ##=$Jv#YH0)3dlG>#/&8;6$tTUdHhuf$'-BMuJSdVu._#9MKfS70UUMS$?C6`aZdoHHiNWV$H>'##ZlN2BO8[`*j*io.6Ynx4"
        "U30@#$4@G;3%eS%3o:Z#C,g>#48EPA^&vx4+&%s$aMUpL%b<C5w]v316CIX-?*OY-$;lv,rsUO'%CuM(A,m]#XeRs$K=Q>#:W?Z$oPq;.%)TF4+utA#g4J@'2I'A$7<CD]x^xX#@I?rN"
        "gkY(7N$%,uh)Vg#Zs20>0MiKu<8fT%9;I1N?dj[8Dwj%OXm+Z/O/dG<9sdYJ#];39vx*A#>I/vB%q?Iu(TStu%C.;1c%HgDL5QlJI;`D+5m_$6J=[>#bBn],U3tp%T*vH0w)f>#&;cY#"
        "&^9[0NPuY#0UO7%Tn/&=J>nh(IUes$XcEA+C+`#TgJ2K(Hg&X?hOw;U`nWS9Oxk&H5w`A7)'xkuW-SO#V.`B6xIN8IR1Xi4r5GF`pc2_G_gcO#3`1W#:dRu5;k3'$ge;:#>dv`*j946/"
        "E@%%#lZFmLZY3?#4iqr$:BP##3i1Z#S+06&/GG>#d9]b%XDEPA;Zud2>Xk0(_Se-)jc@L(Yv(k'umw0#m'4t$8+ZV-5M#s-0m3jL,d><.r<ZV#x6+eutFFI#*^R[unDL5/OR)KtQujLu"
        "On%HuQEHG$NhN?MV,:'#xhi4#5=RW$:g1$#Xu_Y7fPcY#$:@G;',,##1]C;$Bm_V$*T:v>+CQZ$6,Yv#u?]+/5;;$#8jV6N74lgLK;8-)Unes$4vlA#@E'b%i'cJ(:KLB#q>dlJ;2weV"
        "VcEMBquYiuH?Kf_0_Zr-)@P2BdTo:m)<GFVRGTet*T'[K@@BsL/.]QM=x&%#X-4&#L@/),b'o<$:/6c%soH8%2]Cv#=S4.#+PUV$:G4.#/Y_V$/p-W$/KA>#7chO)wIkI)<aTv-+s;W%"
        "r.rB#g@h8.6itM(&1e>#&K=P(?8@Z$][gA#uJ2T%P]80O6h4E#StCfh764Au9D,X.*rYeu,J=58U](`5A?qQb+WhV$XLE:vr6GB$29qN$N&Qr>+9BY,DpE*[I;G##,al'&M1.5/::,87"
        "JX#R0RWo],beiv#)2,##L6SnLG)<J:)Gc>#,%(^#8a3$$-Q:v#]5MfL,p=i*4C6G*L/Q8/q/<a*,?w^#ne^F*(LD8.vQLu%(>ur-+0IIMku5/(S4[W-L31P'+Z2N9JV0-4r?8e4o<8P<"
        "Zd/;Qac6<.ZWHqu9f5VRUQp'R[Y=;6dM758LQ*`aD`P_tq%u=$;oJ:dFKo.$l5R<Ol3.*u-#T&MT&vsIw[a`WC%ji'U]W]+3=CP88j>#-RC`;$-;G>#vH,+#7xCZ#4oqV$mrBh2/Vc>#"
        ";HY##+B;,#9;<PA`M_$'('8c*4O2`+F/1Z--,%r%SmxjkUi(?#tGIvR<'jtM^A7puNG<r7Y)@ZZKNaUmhujo7XU:^#Ns,kbU&)kbh_/2'$xGg3)8>##.;P>#<#+(#3Y1v#9.1,D+>cY#"
        "UvboL>8]H#1T]Y#*aW=&N?T6&f`@L(R_[s$HBSX-b,d;%FIZV-$ddC#G<pau8j1%*1J:]kf[VJ1v]=L#K-YdP]M'NP-?qqd^q$##gS5/v1WKY$Pg1$#Qqn%#9>hI*a9+=$(q)b$wYl;-"
        "A:'V%;T#)<=M,##u`@-)O_@W$:XWP&`>dO*cdA[#&0ZJ(4'rhL/ULF*<4Xb3?#WF3m,;;?Wn>Yk:+-E3Ye,B#rusd.5<D#I^0Q8I]-CuYZ/f:QC<pu>AxlxFG.&tu_a%.$5OT@b:$dR["
        "5&M$#/bdC#7b)##C%K2BV_)20AL]s$YuRfL=NZY#.>$4MD<-##l+Iq$:(h)3KjHH)SqEp%LB[g)V$vM(RE6H3@YP#u]mlbux#8MTS*B4O-NehIFdB'#E@U/#.=a'MeGp%#5>-++AJ(v#"
        ">pfc$R>>##46,##r2ph*Rwg,2X_YF#YJvn&Axu##`KKY$8D7.*qYVO'T>d;%mVOl(N29f3'OOU%PJAiLkS>^QFav%Or+1sb[C1lLm`Y'8bOml$rIB8FH0L%r>V=:`)k_BUd$][9;RY<B"
        "YPgc.xkr8$V9po3*V[Z7g`._#5l1?#QS3.)-V:v#(OkF(h6H^#`]hpL&d'E#C&bq/RQo],)5fL(a;-C#+kPUV&_(%cLeJg::v1@D-*]-=]IE8i[XS]<sP44>u5BFJAU$Y8Aq^pEPGvv@"
        "jxR8I&+o,>mdHt$<<>CI`>'vAK:HWS1DYoM1Z/cG-__2DERh+MNLC2#v8wiL;5O$##Rqt%&/>>#1>`Z#x#f>#(;G>#8xt+D4r-W$h4so&_9N$#._:hL3,FA%t+rB#J#G:.&ulV-ree@#"
        "uZa]F,p]O#&cCSRBI#N#$e[S@wa@huJT?=Jc'b(uEVE1#jrc/vgFCr$N4=&#?-i&$:$PJ(=iqV$w5XV$3r$W$K?e*.QVl>#ZI[<Ub]BB%iSM#5on[Q%*Z,)<aN-%-$<bq.KU1N(&m_l0"
        "^CtM(X?k7%><Tv-[7m:Ap']v5?;Rv$bdfw#?RH=lf47)YssXpPrVfODoq'%TYAm^6J5%f$Xx%D<fciLfHfFG?P%53X@k7k4YT@S[Gtv`#WBT4ohlL8If*IP/FDm89(,,##+;P>#%8]fL"
        "?f-+#1cC;$SmhNX?IJ5BH(Me$SlLv#?Qko4&`SF,dc08.OjFB,*K24':VP##cgGV%u,0W-_ewZTf,Ea*PBQZGwKK/)7lM,*`Us29FPQLNr,=HGg]d58Kv,@]@`$MIh91#J.aaWTv4v*R"
        "a[te$^=P^G.a*9uS4EjaI->RHs,Dteh657_fG>+?Cq1wVEuC-dl`2]b]<Ufq:)ur$E=.H+rK$B#Pd[B]+aWk+Wr/-#^xv(#A+S[#uF3[$LxF)4(RLoe/^@@u8J)1)G#;6P1A;B5Txe+M"
        ">@nJ1g2wx+-27A4[mYv.1_fw#J1=gjVO'2BJ?5v6%qtc%4Np(<&+p60v`Ll0Enn8%p-bp.>;gF4^)Le$lcMD31ntX$%f^F*r4;T.OR39/(JdD%,LLcr;VKUV<BHGRxl$BKtGLXJrY;3a"
        "RAqU:u:pvAi8xrLE4wJXNLjHQ6B[K$tT`G7]L@0GTT0_[Ov,m5jIX]Avb^N:DvM@@#w-`G49vAJ4`JjEG7f#Q4*_eQX@C`#*HN+iPpkVH]Flr-n.Om1RilY#D[O/%L/5##7l=JCtQ)^%"
        "MbYF#,HA&,^[V$#An.o8a4Hk(PL.s$6bCE4fHhx$.)JY%ul/+*E,D-m^:v>-+&K:lk*hOox$aR#Ho8<6OVd%]b7hiR`asGPt,x3O8<tVHE3nXuBHnXb=ST34+-e[:8<)VO=D)eu<jj%B"
        "#*IG9oPfI*/ifr6Re3?#/]Lv#G@HC%8rFJC`b<#-?:9H2V4<.+UFV?#x^dc*@Ch/)X<pI(^Q49%H@]n/UQD.3Xt@[#P>]J&NXXg1K3S>LK6xuL,$5F57DAU9vb*`u:2/X#f]ZC#cuLZu"
        "w@?c4+A-G4p####%roG&4UAJ1aKb^&=vP*[(nDv$>_2>UUi(ft]GR>%(mr1p#bfj9jODH*51GS78oCa>b</W$<,5?#3$ar71bKZ-E9/*+j<iZ/srG0bVTv)4T#`@QhI'F.<k/R/KXum'"
        "&>q?heH@5Uw^'L6w6ffQR5@uu[&B.v:2?Z$8Wu>#*J5n%#&l+DZ=hJ:[qji'a_LS.tRKj281qF*-AP>#u5,##.M1v#9M4.#6J&2BEM=;->[t0(ggj>-wbg'+Bkfi'N-&NMs[+i(uLHw&"
        "um?d)fNf@#bC]:/xrA+**8fHd;C&fKFI3sMtx&*>fF(U/w2tLr]LpFMjlaB>;JB5J`oML^c6K@OpbJ2^maTDNm=8uW(m8`#%i0P85Qn'u=%]<?\?.>>#LK*##^9vfC-eJt-I,0=:OU7a+"
        "J,P'6F9a/:lY)]-Z*xx=2><F#hKB4OEO.EbgT0S.pSb6Tr2Icri;wx+n.Om1/7N'6QRd>#7I(B#-11::^g0c+uj.##gdgQ&R;sx+L*f8%Bp()3%7WW-:)6e4N2K@6BVq'.X6Wp#nhmhg"
        "5MA]93pub?k)rL=tQAa@AngpK'dHHQPj^`Ks*knB?\?-&@Xb7kO1qU`*lp;kOu$[i9LCbT/57e8%=<1g(;P1v#Pp-gL6)j;$/71::=-U^#+WUV?V8r]$Ubs%=m>,<.@e;D+j/d3'#$OM0"
        "DO7C92q(T/OkkW8?mV5K=bIg:8bCE4^m/0M2@qPF=']=gHj'^;^W7J?lPZpN_&9bG^w1,?BU4,>gdxAA(768V9x4=VjD@+?h8%[77d&CJH9w%AsT$NDAOLfLJF-##%Ov?$,Nei.TcCY7"
        "36[:&Ui_V$,Z:v>.n]t-QHonL:H[Y%wCmE5e:/Hd3Ll=u:KdMPh&Wo[+%TaudO&ciq)S,$Ot(lS<u)FIgMW`*GLufC3O?M9*bO'6f$*?#*,>>#.rt9:A0h#$.a_V?(:Ns-QT+oL]^>&#"
        "H,Zs./<CU%p'%N)3pVT%@P0N(p@?@u:E1%8G)HYg@B4=<`5B`ALxSmFx)GINi$@huLC^_kX,voGrnY$8=Ax;V&Yx[#q*sE-14Cj2]Q6+#&(n;-6@e/*KGU/:nBLo^%_Xon>8ur$^#v;/"
        "&/>>#u#]>#pjR>#k&6O'p^0<-CQ$H)pG1l0btQP/(7X%$]+23T)[AoIa@ljttV#sQg^N[.K/W3;_%D2:F6Z;%s&r^->+Z3;`24_RR%QX&3H:/H6KkV0+E#=:`1`s-?l_;$]G.%#f]:Y7"
        "*uiZ#v6@G;5MP>#r5,##(85##(jEP8Ef9$-)#sO'DOI@#=6Wb$.v3I)UXvP/rOj4:4$?^=;JTP8'a4juY'Ql/cWFG2BeU5H+Q9)ei&F&AncXMKst'J$s)tpu'#DxbY6Z<:CMO-TWgWTM"
        "DLGf:D>n_?wc#5##)P:v@;&cr?$RYPw$)j94CU^#1;,f$A)m<-MmAS'H[ai0._%E51f68%*AG##PY=wI^G*;&E7lM(69cu>wJr)+%SbM(xJ@^$$ZpM9`n;-*J>#G4.S39/ix6q.?6aZK"
        "x1le$971d@C96PMC#(g4#5-G42fA5JbUOF`c6K@Op<:,`@MAa3>UPD?L5)4c.Ew_:##On:-'i*#r;#P&v&$W7E#<$+AZ)6qBeAj-d,@pW+u^&#?:MFR7I`xt>ovj9/`?dF;QwA4ig'u$"
        "8i&mSV1v`6L:[A6D&l^g_V@MK,lQ<L,lvsLN//gu8;@<$J`:Z#63Y&d/R'#vS2Ki#l)V$#Pu&A;1BVC?xf>>#)DY>#7ON]=_bk&#WCse+)2M;QR&xIM]LN3Tu(r9Rnlq@-n/8fM[MR:R"
        "5*qXRiHVPM##rg.@8%?$Fh7m86VYH33*U:%nGUv-/e;p$TD]]OiiAqCnv2-OiU[@9LvFn:`%GR:j8c#@cp>vSvFc,@:.nDHlGtRDQ[&##`9`uPG`kV-X7vr$MnEa,,J(v#r-@G;FbJTA"
        "(5G>#)wYU9<iM]-2sus.TQboRL)[V-3o1%EX88iKYW^`3AsB:JvJDD*[mYv.Jf5##/JG>#4#n-F(WpfDJ,@_/:&8W.L?3FN5B;W-jLHx6V3=c%M-$30ds1ZRnx3U(E46hZcDbi2;^_?J"
        "4M4M#4E=<BQ#3=B;[)O1[N?i;g;[9`TKM#5sTRP/GE:B#MZ0B#3`u>#ImLk'.`U;$O]$q$6,eEPUac(JkE<7%lGv^]QGl2(PnZ8T3QcA#AnT2T3.kB-D0lB-*pS/M^axI-3xx(3xc,##"
        "H'a`+&2oh(cOHP/ENa/:'^,g)?5[&=6`j9B(47)%X+ccDck)K<Woe-RYf$Z@I:,/<XB0w@Lm,s8qaCO3O:]j#DhT.4oIQ68mhx14UbkT/5_C41xge?\?Q?'G5T2G>#K>uu#E_#3#N>cX7"
        "YAG>#$0@@%#.rf:0i-s$+/,##8>x-#32<PAp'7G;n/-E3PgqK(k`7L(x`Ul0akY)4<DJD*o+0J3.W5x#kPU.U@ZT)n:UCOF**WxAsQO_uwf9JIr]Cb4?a#pImA@A+gU>T'@AXv//G(v#"
        "+/>>#mp&8%/Pvu#.`Lv#D<G##7%%W$;HY##0c:Z#93G>#/oe?K7v9<-l8U<.Tcr.L5U+t&'cugMT)Rb<1'M_0=1#HV,W<v%rf+L>i'Hn<wL-1#iTss-$Let-xHx8%O9@K)uSGW-]mFuJ"
        "1P2]-ACKC4LZ>x#f2rm<K4fJ=io,k+PruI#v`/i^Ek#RMnM'p2o<rBHYo%@#DJL--;9P>#<W$i$pe[%#QtVV7Ku,##/YC;$FU`V$c/$I%o=vu#'D(v#8jA>#UYx71Z</W$3Z]Y#7U629"
        "&hLH.[#9F.TRVC?P#5gM:%g0U3.kB-*,5gM@IG1UY=_QUVHVPMRxXp'ABx],K#CV.^>:Z-wYLl0:GUv-LshN0ND#G4ae_F*HvX)</n*H;%P,B?wQ2hD`<e=99xmnUitP2G4n#.j;);O9"
        "GWm<8XvHAcP#oZ?Tu<I5e5;>Sb$5]uGe5%?U/Tf3%2G>#WEf9$o:n0#63&W75Mt;&84h0,K3A>#[MUpLJ^4D<OOI8%wjGT%Xr?BPrAS%FKw,[54c&ZB&>uu#4m%crLKRYP^O18.xc[1g"
        "Ytea+$8f^$VLBL.s_8q/amL#>mmAqBQ*#;/;^p;-3gl3b7KMG#Uc*w$]WD.3xsFM=h^UDZH*oO>7I7mUd<1ju]juJ4.&Jq9ctcwHGF1r<TR-j8#$_.;CMG1Fh=eR(X]7m:P/A_-8Ow9."
        "TSvs.9<c60CcOd=-4n0#[kQ>#B^4:$C.02#T-4&#=J$J*,V:v#(AlY#.d,##n),##,J(v#>HcY#)TUV?%l9^#fTf'&fu?l)(ctI)Dg#s.m5Ea*S&S(>/Q#6:$xXI)tiv*3YVFq09p.H#"
        "v8sH/cr26C@@U4=YQltufXYX@5uDC9BA&4*d.&8/C>W6C%fD<_OkIS@w;RQ4`xi'#GlB8.#%_]O:CR8%5M###TIro%:]o._1Sb84N####_<iuPp4pV-rpr;%2B<.-D0a.-o>f--,@6F7"
        "]>^/-LZ2_ApH%.-^A^/-*3wE79'QG)W^4j(.5v`4)@%sIZgO/)uW2p/?ejJ)0$$<7[0E>Grb.g)2C'a+E=urR(8M,*B_C#$.M'9/(LvuPSidG*af8<.7Vb]Oq=)?70Ls:mu**W-5A-?7"
        "44[xkYqgMBM8Ym'[oYxk?_(B#GA:xNoU$lL9BXd$S4S>-a8lV.$DT6$p:S>-^jS>-,]W#.jG>&8YTn`A<jwE7l*YwK*joG7S,j_ASQ<.-qdP:V[GSlfSYN2B:n/g)p$Q`kj^V`kQq+a+"
        "E=urR(8M,*cjD#$8jw8/8r#vPSidG*af8<.t_^]OWF%?7VB?&8Tx&vHl6_#QkooV-K(.?7DK4v-i2w%OoU$lLbd-TMk'2hLSbS>-LaDE-p:S>-^jS>-,]W#.Ts#q790SG7<jwE7l*YwK"
        "*joG7S,j_ASQ<.-qdP:VE<*,D[$6F7,poG7vjvE7RKl9VUW<.-gWrF70k@DEY^%.-K/f`AnE@>#_1qxF9=2Dj,cqr$/4>>#SbRP/Cbw4&%e&@6Wc+f#;#^Y#b/kxFukkMB3:35&Ya>Z."
        ";cuY#*8,##$B4?#7aY5&86ce+p==E*b:ae+'vsn1SFjb9a*)fuEF-g1:w.`;qKO]uiNbv/&8YuuvI:;$@btv/IS3x[h.`Z7`fC_%9vZV%?BL,),S1v#vYqr$*DP##;2YV$.ihV$=s]Y#"
        "-Kuu>*P1v#%V)I-Gh%##B[m(+f>Mk'%0Wv$Im7nNT_`##;5:1Pta'u$Eumq(iHc6W),W:d?LUp/n5cT;`<lT8,`vqA5hmD[vLo=_.6.O;[BWEe'4-crA'-FeTk5GM$Zn-d8OA8%_HNZ#"
        "BR.@#dn1D+3%.W$.re20VVcY#2o:Z#vx=$^X+@s$ED5?#c8,W-KJ&iL`<W;7u8BS%OuVA5_il2([&1gWT^2E-<:DhM`Ka_XT^2E-@(d0MU6_N-='3;6?tgQ&&cNn0e]-<.8(U+*Ir7*3"
        "/lqr$bG<E*B95P&gW9j9K'-/=w6I6%(tTv[Uxq17XmR`c^F$N;)K;TJCSA.<XMdg*,;^%<,+4)#&8Yuuwkbq$Cub.#Uqn%#^Xag75ik.)XZ6x.GK:E#4>h,+XOhs-`Ko/1^7n8%B[xv$"
        "/*`-6M>G##n.SgLYM;s%LqHdD2%78%Lq6dD*P1v#5Q:v#`Btr$:&A-#r0>^$E6<vY613MM1m6b<Gx184G*O/)MZ?RE>cK84MaG)+KN-REu0oY?xpUY7]Teu6[Y,G44=#70rhEE,nkUN("
        "ff8B+0Cpp9.IRP/7*t5CN+n=l58@7/XHuA$PZRY<6Qn?$jA+wAQ#3=B`N[*=MQ=&u_t8##ssF*.F#,##nG=KWk2^%#G=V/C>;Ju8hGZk(D%rV$8u(v#wK,+#5sddFa]d?$2v3&%8O39%"
        ".]L;$=2YV$4g.-#&_ED<9D###l=0'+(V'f)h5)O'MoFZI[Di/%`+b?%JaOD<nrkA#:,pc$*XuZpwOMiuJ0jQJ^lndM4P%V'<@<D<4vk&#xc8xtEt:6^i%LP/3]NY5>0'RaP8qqpb<#R0"
        "H%)Z#`^G70bTbc29Ies$YZ1pLoi?(#0SP>#HMNT%>PK>&DBpQ&:O3T%EVqo%<s.-#2-kD<'Y-##E>5_5WV,G4A6.12g5Dx7k2Cq%xrL-mHKWNt)wP&=Td_#$NR2r7NAB@6ch)K<Yx*IR"
        "XcqY@H7#/<sb?&%j1e1D*FqvRkE-g1hW5E,Jp]xa)+Mc$<l2'%8KA/=63%60QJox.fjjb$^FHDE'J/%86<1'#XXp6v44S08g`Ys./O3H+5uj,2;@*9%Q1wS%>fqV$:M1v#>a[#5Yfh;$"
        "O^oYG0u68%JXU,D/`:;$H_qcDnMdE&RPuY#3BlY#TQ$e%kH4?oxkr@-aisf%JZ?RE]#&gLE)&W$@(^tVq1(-F7k.iME[8c$FN-REZkR`gwL-1#WvT%#4)l>-D*&&,.c0f)Cn`N9-wK/)"
        ";1s=-^q&k$;'vA$lWg4=sDiv@e;91=ZLC'#&8Yuukkbq$vY?.%[=;`jk9JKWwP#A#juZC#Ws'B#RdtA#j3ijV0(S(#>OEp%`$?b7`Vl##>MUS%>nWT%H`65&:es5&xIAc&Yuh;$7a_;$"
        "ThadXXeH1M9Fo8%9l5M6=a7I6TRVC?-[oiM0^d3^)]1H-]doiM6,E4^iM#Z^fIVPMH`t5(BBRL27m,W.b<2m$tJp#e[[bL+/8'Jhw3aK#l'sehGGx#7p<WT14;QALt7VF7n&tiM*ef[R"
        "'tiFGB7Pi:CAMrm]Mc;-fkxs*`kn/N^UZuu;cFU$<IU5%o/s%O]9e&m:``B#(1BE#l.eC#guZC#,R.K:.`Zp.tLNT%[,>ZG&2>>#D:UcD8ONP&V'%-D7:RS%T-@dD-PP##+*0kja<<v#"
        "7m=;$ac7+#CuS2Bo=b87_^9H%Mu>[9_[39%khlB-KFg?.rkj]7=,j-Q_f.(:pfhG*@,j-Qex.(:u=E&,>vV-QYQBv7UF?\?$,]L;$.3c;],7N3&xR61#b`mA#B)qv7t*+^6^c5G4RLSn9"
        "I#cA#sQtT8UDsS&>++s.sB-g1Hbn&dWm8T.*)EU%T>uu#'M+crEW9sHCEcf(D><fq@I/@#?qNX$8n=/(eQ830RoSB+MPs&+7@[s$&3`>>D*>)47'k8.7RA2'3c:>]M=$##B%l+DmP,R8"
        "'SYD4_*%ZY@aNv%6Hdg3adtV8GIRj6Cd'Q]*cCH-'+NlLRru1%qx###tIaT/aS$<.F$a`+UD%8@V;=Q't+rB#(JDt-O[M78ag6;[Oc7T%CgndMX6P=7dwcVJ+n+xR&tiFG@.5M:B52Vm"
        "'kA-8[4w-$uiJG&(WYGE+oF=97M:'#;9CG)cDuiT'>2T.xG?C#5lB)&$SdY#4](v#;7%s$U3A8%TKcW%WAP##/]1Z#C23p%4%`v#Xp$q&EC`/:uplS/+l(X8vRM[-Hb-miE_s3:&h1x1"
        "2r9aWRW2E-_MpjL0WDu$ePms.xC[60x?(02D_m(+Md'e;(iGbY*iYE.VmTi:k,U^Y%e&@6&nIV^mg=c%4Mk-@AaBN(LS]Q:4DwY'%+5+@lAX)#4O/2'k7NDWO6?A+$#nx49QA5/Eh]@#"
        "gEIA,ZXsYf<e=D#QH=A#Vj'B#sRWD#3FDj91KWZ6)AP>#P%j%A6B8N0.iC?#Af65&<n&6&FSUS%H*#n&Ji4.#;PaPAdA;#53.,n&ADqo%gDXj9*-3v67Es5A6LsC9F],@9TRVC?\?WljM"
        "KYX*aDt*J-o`ljMQ(:+adBW(8'goDcTK)]5hU`D5pODH4,Sw<.Yx*/%(Glv,dI/+,3LRP/+%C58Ai1^#TCX@*bQx4&'nA[6xd.;^bmiq:c$;0<6uqWVS)n.5]I,IB.BbH=LT%'$KGto2"
        "n6V'HA_kR%UjQLE(SJ@8^7[Z$X6-crKU>)$*Vr'#%HlX7/)[I%i5BE+C1.W$OhG&mqS5&#/i$W$D4-(%))?<-_ZgZ$N&IjTsO.'&JBohEs4GgMNAwb<'1B*/deFjTbhd3Va9Bs$`Y###"
        "=XPn06V=.)r0_v>+0?v]DXR6%dW)B$xI,H=eN1)ZH$fO>6I7mUd6(ju^p(K4//f6:7RWm8DQ^VIF%99/c3d7v7h5^;i1YT8w6FV?)P2E,+&IR/(^=22c[NT%F=Og1U/5##.MP>#>Nc##"
        "2nwG;jRV8&aB4X-B'@ha`d5P:-E#p49f1WZh^]$&[4,XA^)2M;,E#p4x(wV729*.&IRIr7@VwGXCYr]7BB(;?V/3=8pZhL;PprS&9udJC@nC#$V+Z58AulA#Qh-a$<R0p<8`j52d;XR:"
        "ug'9(bh)<JCJ&i;%?bG7#4@&6]Us5/ChtY7Yc;C8#WfS80kBH;#IG]ub,760(PUV$cT-crZjLKj27f(#^LGF#EO6l9'cUQ9u#T&,?F7<$*Bt;-1MM')OA,##GLE'%k8$n/J.^2BqFk87"
        "56N#&OAP>#Tcr.L&)8KU=13MMgh6a$eCh'J05_S[[v,N=pxqM6$ZM%@CB82M3?c,4d%mq7cOnHH'g(Q/^W;R:Q*2k1ulp;-%(`f-:)?[JEbG?%7jE.3`u-m)ww8p9J=cAu5G-g1Q,L#e"
        "?%6q*S@gW%>3@(s-g0,`:)ur$Oukr-4Snx4Hu5N)+/>>#:`(v#msJEY:@C58tS[A,?>EpI.h)g;#+(A(&%ldX2PQm9<'rd._i.NT<)E+&Y<:X:=I@0-<g16(0Ebm/bQ@O(aio#,MRg8M"
        "g]ox2hiX-GAxD+V%e&@6@UCa-]vp565lAvBUl+f#mi*9T430>GGT$(#Db;V7I+dY#>0gQ&t$VJ:;4F',/DP##0fh;$NaF.u7_dl(,kD.5U77dEwc1H.'7fG*?wB5S3.kB-iI7dE>)CE8"
        "Oer;-$/_d1jcNi(MeUj(x`Ul0/57B7x%5c4C)C>GGgfKG?RK301M5j#=xhk$C`O1+m%@SF4lFJCp.(m9n&C=-0BIl$S%%<$E6%_#hGEp[49:n#Inuk%Y%h,M2$UW(K]5]t82@8@O,C8I"
        "mu9kF4&Z1T2M>YuC=Ld2HW###;VOeu:(q'%RHY##7bi>#1@4[$bovC#P11,DPp0J#,5jI_[KGGMJ@]b.FL,F7>eo929Uo92<rnY>h@,*3*UOm#Lw1c#77>##?doW7%sRfLLEdu>'r''#"
        "6&Pv#goZw'ONb;$.%[8%947W$NZI$$X/*E*cr2]-4^k-$JXZ%t$wZ%tdR'vLOjk&#kU1f:TqDJ_f@TY,p:k22,]U;$u,=;$-ih;$=8YV$.gqV?)%Da#:1(,D-PP##4QUV$f's;.YP6W."
        "xf_l0K$/[#o7CkL2o8gLWvux6TOr_,l(TF4Rm)K,<$eV9_XR42l,MP:/1N-$Kwg$7dYNkD.P`w6tp*s6^+WC6M`E50JP'v,K>)C&nK&##%0Vcucp_$%*`''#+o*[7xlC;$)Bww%c:ffL"
        ",k+8*Tbv>#%[ND<sI-##q_SQ0]go],/M=.)K()Z#qH75/vA(l0q%g7.Q-SO9`L0N(Y:61M/gl8/oUFb3sHZ)4LA^+4,D#G44o0N(C4D20w%a:CIDNnDv/I7MGDW*Gu]HNL11T$0#p:1$"
        "gP%^OAakcu89@9)0L7huQml2M^[VK##@gm2'Tbv8ZHGWLaJgB%mwo=ur%F]#&t.-#5f1$##FC=%r&C0cI2>>#Cl-=&Qf:Z#@J#X%3lQS%K-B6&Z.s60^V7L(wSCl0cDiNXuLbA#f>-<-"
        "@Z88R$K%Gr%_^GnQ.q4t0$.N9$Q/N9#s]xt0/bx#[BHX0fKb&#@TZ^7bGFgLUwOT%Z(J[#-VuY#$[n`%eoqr$KU`<'TrJB+Rs*I)NZg6&ISrZ#KMQf;,DlY#9U>+#@xms.X/1Z-c3=&#"
        "D*&&,xnt2EmojrMnfL:%rq)v#<Q_#$Sp</M&t@X-Wp0.$:VE%$AG5s-emwiLS:w[-`'YB,6#fa0i#V@k3c%T%U.Wl&$&AuY&pVPAgVMC#5bB;2%tL?u#JFwP.LO`u<8BiuBr>rZSOv-$"
        "L%OoP5J1Mu&N>bu=VUA4A#gE#4%###xn.1vnb(?#xf1$#`;N-M8fipLO0(E#ep:^#?iDt.HjE.3$<4Z5&tZ6;1g'R;b2dZ:.YGn:G_Auu_B)8vSNqhL]JI?$@8DmL;1&<$Kkaw95xA)t"
        "=Z;W#_.JY-exNk+>OP]u`_=Z--c-F%KG:;$.6CwK'j5x#E6>##=@%%#j%iY7gJG>#'>Y>#?D>?#,SL;$@xp'%1u$W$<m_V$FdF)<+/###6%U+*[KK6&-.SQ/wG1l0:_G)4A2e[^H,F$$"
        "74+2$q;Q7t=T.m-<8H(&8cdfQxQ3,R(Zk3kqkRF%rK-#vdGB%MTsIG$JF)w$Dr/SMdL*gtbq8gL7&sF;aWI8^g2wx+*sU`3HP2T9Y.Q>#sE,+#+MuY#R0[0#TlIfLbQ$##HpTW$4Y:v#"
        "3m[?g(_=)<]ZJA,-:gk14?*e3YMmo/@:1f)VAsx+g],?GKTV-*^$Jw#PkY)4bV$vG3;YD4]d'aEj-=x>&te,;'xT&]LQ]OEs8I0EoocXCcGtYIlUZu,2?cb5NM4($`l51<.GBO;0DEGP"
        "Knb/>utp:9Y&E*@dwMa>k6=C?eL#iK0<3J<00e.<6U$(#if>jufnxX#$sC$#)p<pJA4hW/$5voLZh?>#.*,##um.##?X/2',knc;/aD^4-W07/pnF`#/kBT(*]p&Ba)GJ:2cJj:5*KM#"
        "wd7>8/9P>#pcVs#u+,>#EXI%#&N$N-wnRj)>c+'#/$###.u'I,<*_&$5Np2)17(B#/&*5ANaSq),lsg.<,r>$s(rB#-aB<-H$=30&4^v8k<;W9SU]F%Qg;Z8g2RW8TDDuGg*$$8&.M)v"
        "r:H&M)E_kLOKI?$BgPlLf4&<$uF-iLN=?uu:OrmLBnCdM=q;UQ_U&F.lZ'lM$M_p$Kaa`shqC($+6eQM7]p%#n&U'#@It[7t/,##,PC;$HRer&-V:;$EC:,D<7es$EuY;'1oqr$hRF,M"
        "-dcp%DGwo%L$T+,P:gC45F>R0XZS&,FBo],6=Qc*d#C-mvlA^+XO_2i@LU58V&T`t/'Br-<KB(&&h+Et$_`CtU`BXQ/Ikxbew>Z8k<ZZ83qR5'ZXEm/t-JZAi7JZAlY#<-#lhl/%v+##"
        "33mR$slAr-]n$ktp)2[[5lj]7S](v#)J(v#o1f,Mb<#gL9h;;$1x$W$]p0P%_?x(<1[ndNBHRU0)8>>#B&[8%f(NJO-%&W$g]AV/&7Og*t,-n&Z$pJ1&2oh(Yv(k'1cj8&J@Rd+33fX-"
        "`S.R&O+]w'WaHS@hc2^@gUd&%Sw7PuSAvW-3$NQ(uf>oLrHL58%NIAc3G4q`uGY1T/o2T.I@#;^TVIm-Yu#HXf5cX$>WZ`*M&Q<-,80K/C5,##Su[fLYPO;./^UV?N^CelF]r]7PFV?#"
        "V>mn&jvp:%8W>P&w;LW-L4N_&s.Za5857>8w&hN(+]p&BpHAYub[tg$XDU(<4ro/;4$BM#$q[Y8,$###&OqR[`wS&d[@T-uI_IU.BVO*vBu@T%J>uu#N,mOo;9.`sEj?D*b+X]+,A)Z#"
        "&,>>#9WuY#fU&E#oQx0#$UOp%<bo790d&BHGP*N;3`gk;;QIF#vo;D50i;D5SP.s;-=u9)FRUbG065+54HYF59=U'#&8P>#6dVs#?lhpL=AW$#;uljLkUFPA*5G>#Y3o>#xU]/1d0r_$"
        "21NP&Db3X$KD,d$?N1l0Mp^#$#`6m8Z+3`#<GMW)Y]gk;/JBO;Av.fCRs&oRQH#30#)P:vsBO4oUs*RE2-o`E/Jq;-7^&0%o<Deht+_SJFgcWJx=V'#?aVt8#1(<->.%w#7&?f;Lk<t$"
        "<o_;$Di69%Orb;-cLd>-^3OT%_2Aq*p_wp.Z&r,)j]daPevk9%Ct#Z-LZr#[EeCe&wEn#MrD+T%$rg>$xEN/%b._9NRTlcM]CL+n19HU%eH0#vV:2o$.B%%#)qdU&'.rf:oe:J:0fqV$"
        "*J(v#kl.gLT=?>#><Nv55JBB%%),##PW7L(jV7L(cu=Z,&'LB#''*a4%rBB#rU5g)&ej-$voGrHmY@@u`3')P&U?v,HlpguEm)<u>G@MgDIRd8$d;Do&7Yt7]e+Do85n0#]_%:Hq@sa+"
        "4Ocv%G1iV$KTI0;RPWC?7i2?#?A4OORPG>#cf$U2=dC;$<P''%JMJ#,&J>V7X%KgLawrV$G5$>1(V6w^iF8w^PftG<n;6L:mk+Ra@PPp&8dG3bIWl3bLFe/)4K-LcE=fs-nI4LY&$&T."
        "V1R%%t0D@FrOr'MXn[Wt@i9/_K&l+%Jd9hL]l@iLDH?pQ(Gw`uOoQ&N,$R>#lN6g#?\?H;#=(V$#=aZ/.RhEg)=`G*RU5>>#@T(v#2g.-#gU/a#Y-s^$>*]:.,N(v>BbL#$0aQa0OCV?#"
        "JdGn&<X)n8p^s20og][#PuYL<-JBO;E:Ku*=S:+506#f4xQWo.$eRSR%5YY#l)U4os]a?IuB&Z7/lS8)j-9tU,;0%%UBr;B3HJA,,?L0Ep1Fa*VPq/:Q'7HFDr88:Up?/;mbvjLYavu#"
        "m4R4oqSj32xa:)34^DO1M7h`fBdhe5lL)W-jt@uibi-6/D>*5vV^-iL$?G)#ktu+#C01c5qeX@$t3@G;*>cY#2f$s$_7Wp$q>c5&D;ou,:1BQ&9Cn8%@/@8%JpX)<2I+^6nrE1<`-a*>"
        "/<M5:l(h+4v(IW.kv5n&GR9;6HF]r%V.3I+cK/[#O`0$Pm^o8%?/'J3e7#`4DabAdK(@^&X3L`JeabF=PH2l:Je@*+S7rZ9?;Gg=If5t&t]R2cr[o7h%EBW-AIn),%+^LEXZlTBc>D:B"
        "N9L[@)0Ys'Ap]I#5)s5:u#m9B1'qE@Tu;R)A)>;$rFR4or8P8I`C5;-xA>G2wfOv@]IM;$sE,+#54Is$_;lB&+Juu#B:1,D5JY##:47W$9BlY#92>?#:Amf;>:w8%?;US%Lvb)<[/(Z-"
        "_+-##$4:K2jl$W.XAUv-HT=>-bQXtAf7T<&_r`n/<14D#<7X%$?<6*&;f>68<,wZ$=&`0]F9pU?Qnr?-4VCf:64f%uit@T.LfEpuJ`5-0jh1$#s%iY7Q;qd1w9@G;/Gc>#,P(Z#ZD)qi"
        "eZH&47]SMBgfeV67o.n&X*&n/<N/&,'.&$73[P1%ug'u$W4[5/nYVO'h_gb*<%:B4.W8f3$LR+453?J<0'7M;aNH0ES1]U;?hcsIMcLT:2/Ft8_]4a=UpfP;g.(<-9OPwAee+uAUoB1E"
        "iO@uuxw.P#,u_v#7&%<$-6YY#ix=4%-ULLjRR?>#D9/J_hNFJUg8`#-Xof+M67voLnbt1B'/>>#vs.##/9n#$orqB#k6g7&a>RPD%*WqL[x,J__Wj0OY0,)Ob&Z6B<Y_D<f3OM'A%Mg*"
        "7s3w%&3Fv#&4;J:DNXrHT,<E*0vD%$x2V,)WIdP/BbK]$5N;D'(sL_uZOGem12FtuQ&]lJm$;`EfV6fu@k$)<8ClH#QPuTNXo::mBa_A,P(>T/ui;+Mtr]v$GYL&MF6KqdtMIDN^0l+M"
        "XvmF#XLn20.a&##SA$pIZ@?_/Yko5#`85/1s[#_#&tso%c,DMp84Ah)O$b&-rueh(5K8$P:o3FN-vhDNV/A8%MpKB#RR4K:;XR@#Xsf=uKlO@#QVQf(Ww`iu6HNkSY6ar$K[Ds#KY,+u"
        "?YCRuRh)WuXr5SO`=PJ#BLwPD8S]LYsxMWuY&?1p:pOQ#k&N9`@Qx9vU.+.%.Ol0.o3TkL.%(.v)]AN--e]j-9gZB]S2YY#-2x1$-jl##@2m/v8@FV.v.Q3#9YCa3%/5##<H:;$%'A-#"
        "^3=&#V`uT'#-'4&X'N?#fHPi;Rh@w#Co/QAC@eS%cuNC$b8D>5td%##D*&&,hsp$np>_q%5gY9.mMO1)2cVblrnYV-E2loR+EYT.Q`7/W^$l0&tDQUg0Q4L#,1;=fZQF&#TDH_ukfw%%"
        "0f0'#.Q?(#*Za@.DLIg:/DP>#3SrZ#.I-_#)DP##H;[S%<AR-dtCn8%C8VZ#EDQf;)5G>#ag)u/4?RL2&r]q/Kw5*=e9kM(9v:d*0Q?-d2>t**j/_^#qnbX][32a1o$L_4qX-p.MKv)4"
        "@-oTiY(?cAvHw(Z09'rmBnvk`qu^@3[xpOq9lpnVvmF1^^V%E)r:<RMR[2G`l=.Wt553M#T5$A=t&vqrCZ5/1(^:T_sRv_umN4KG]HJ3%(7kC3I+KDN3$8-vBtF)#q4,a7^4PJ(Cc-1*"
        "3P###Q[LcDFHGJ(cv<eDQReS%4Y1v#DF1,D&8YY#3qrK=1'Zg)Yk3p%U>Sj%g`C;$?N=)<4i$##t*+^6W+V@6[iPc4q>j?B(h:O1(=*3&-nw0#P+D[,sXYv,1.*u-YlN.)]&e)*I+7s$"
        "BW/i)tmVa4oIh.*SO:E4)dkV-34Es-T0wiLR7`X:c6(N0clNnDm,Ew$A<[*G1Cc<-Pt&99bd'^#fng^Fg0D'6LGVMKYk1F%Dm<8.A.D4%=1KP9DT1XKZ)Z20*.G*,6OZ_J7f+H=K%9;6"
        "ZHMCuk8[K<;OUq956^5/UMd$vN0S->w+qNC'dml&D<^m&eW(1(<ZGQ/F$J@#^'2kB<6b8.`9fw#R.4L1d.$##efHW.T5U;.$7AP)(WU:%.^vJ(Kl/%>qRH$$x1YV-t,kxFglfp'q-R>#"
        "o0SP#T_C:6qTr=#.LO`u#NhY5+]p^#+n/Cj]JK8M^u+O/Q-w0#k>du8&&m<.Fwt@X1A&c<[)>5(W9qD3Ixt+DHZ(,)]pn-DAXjp%Z'6I)Ye/30F>ii'J5/-#Lo5;'[2=V7jxL$#b5;k'"
        "K-E90,DlY#=%`_?*>#5;B%Ak*-PYv,G)x4;kG;B'd5(r.jQkA#23k-$h7x`=pKU.2s7-s$G<jwt$#3Z6[Mg;$VQ=P;N7e#'unE%bV0j+`drC%beoao7VF=+FO<Z]+vsai0ldQfC(EP?#"
        "t6CdM^N01)mZO6&RV3I)I3Pj'1Mj$8r%QgDHZ(,)9n@N)xLx-)J$2.22n%##A-ik1uX/q/Vax],O2_X-*TXTs#t7HM1_Iq.$rh8._^2?%K4%j@juV#A]Poftrx2Z6?VDSngt<$@v.Ss9"
        "kOdp'.eG*=H9_^u;+YQur0JZAh(&?AQ4[5/a/R2B(?cu>bNot@+2<U%3+@W$=[[[#^v6I)HSU]&^####FIsQ&AmDA+AB*.)I:nt$H;&^+(Kb;$K$Xs-LV2v>oO6k(agK:%<=;8.m)bc>"
        "4Ynj'&DT/)g/q8/]rjgII<=L8fs+Aug(wM0c]sL/4Up6$3Sm$?ve:?%Jk-M9M>^=G%&###MXxh;2=T%6X2t+;1X4[[wkj]7oxL$#3+i?#98ov7olDk'i#jM/G>?f;LEAk9$k9(,En1,)"
        "VqNQ)-GlY#TIse+&PhG7co?m/Nvc3'I=P3'6=b^-<U#I6qeas-WRFG)ei$+<*;It7UPYi95Kdw7)++^6E2<)#LB`41:U:>?[;dG*JJvu-:oxh;VZJbGfQ/[#`CT/)=-Bk&1@w;A&J#V-"
        "H_nf6)7pK1)Wv@A-/2O#_@v_jE]k*>HbOl*7s:o<%K:S<(xj30*taOF$Ce+`=91LEQ7L]t9@uu#eFbK&6qXpgRPFh)LR@<$_`nH);Yc##[s,4*PFr;$==Mv#)25##McX4%lT9HOlXi.;"
        "@pVC?UZL6&C*k9%C]U@#i=nx-=)c9UJY_)56p0<-EJ2qLA*LM'fH@-DMC%%#[;eh(lTf;Q'Rp`?/#@W.[x###e(<T/_DUv-hJ&)$:U#I)^w)g=*d_.(Q<fX-8DT/)(-,F%MMSN.:up?Q"
        "bg+`25[,e[4<p,8tFso.-*#MV'*.fCi,A8.)4M4%WKt*$H/VeL$ZGSIRR<NL]^<L#QJa/M5G^0M@crguW[*dr(h^Y#<SY4S/?M2BbOPV-Nu.Q:F.iV$9MlY#?7(,D$,>>#6IfB-hVQn&"
        "@eO8&PP:v#[-l_-eG0-'C'R*Nf+/L(IA=Z$:ru^])Vg/)uY3<.<#G:.g)Zl0uDD,MS0hcu@QbY1b[TE3SY%/W:xeH#_@v_jPOm(a$&Z]=Q'f<-eJ,hLLL-_#2E]4Sg]X;Q6M7A4eNZ)>"
        "6OJM'w`;O10)^q'BbS2'+AcY#:>x-#aENh#k8>?#C8?f;E(iV$Bx>?&YB@B29t[Y#;B+)<KvjI3]AUv-#8LedtB/3Ms/&s$f#_:%x?TOTE?mV-u@ZP/OcJ_4LiY>#OTrB#Hqw?pZ%oY#"
        "$`7SS1S&[%^pSRG7KG&o(GXUUw?-+)tn>_#^f5cV)Qg/>hvaoRHK4Aurt$p%'Z&Qm<I0)*t]:T_clc1K6bD>#j'4D3sb.^=6$XpBK`qR/a:+$O)H%Cf8FLM8#)P:vVO3D3PU-F.b<IAO"
        "fP+##R>$pI;SSS%2g6S[qO&Z$pP%fLe#*^#Q5T9&4*cm#,.'o0pmCt/AXR@#Jsf=u(TR:09E>>uLs,AbFAMe?EwlFMqfUpJ&K4,;Gm_A,clkxl>BVp])Yqr$&`0wL%h$Lg]RrlK64lxu"
        "x$vhL(^PbD):Z;%L^wc<-Q$9.wfJxtr@uu#?&A-#5-sV7E2,##va(($BvMA+s,,##76o(<)vbl%>N:l0[W'B#IDbbFw51Qu-4vC^(1P2B50:;dpx<b7M%%X-r>?-dG]3D#sN<.$q'-K("
        "b+->60QmkFELlB$cQUV$0Y8`aEhAZ$As)$8UpNw9fU0PA)?e>,Ie5r@Egw#6?nTh(TN.lBdW<$mM5?Vu*]t]uaihxtog^B]+ODigZf>]kUCRP&`<AJ1f:KfL#LYE$d)M#-YbOc%]Tb`<"
        "lJ*ME%X/(6#Eg;.*W8f3Mb3:.t68,52`m(5XQwN+%lL8I/3e.</0[i;T@+MEx0c5v=1+GM-$;'#8Y*a<,j=_JHR=_JAXa`<hlW>-lShwPm&j(0?AX>II+OFY06$x1%@49LgxA#8j@uj;"
        "I1X^@)1(<-x-?V9#&>uu/n=6#+-lU$Mvq;$Q5C/#4L`Y#.4b84rPr.2=qMi^HeN8I$2MJUg(058.Y-W-_l]h,L1$##2-1R#fCL3$Ag1$#M2PX7S;cY#(?M,#.S(v#:41,DoX>?$%),##"
        "S;-E(cp/1jWQm)4YnlUZ*Eu5/w9>>#Af[k<(,8)Xt3N0YJ##>#_0ei^?@*<-uAS^$g1ZY#1]U;$Q5W,5)?3s@>m=(/Wuh;$/a_;$-l`s-Qr<9@&BZ;%)f`<-c]BK-Db=8&%iep.6eM##"
        ":epA.W9pV%eU7,RT#0v$bmSxtU/bx#ts.-#-Mc##pn+@$'8YY#;+(,D,8>##r^Iq$&NUpLZXTiBF9(B#?9V-MQ0dS7/f?>lB9$QA$Q/N9=/,Q/P)'v#@N%,D&@E;HL)6p'xSk4S-)Wb<"
        "2a2#$,/,##)8,##Cp_V$.Z:v>*=HZ$<J_8%*%:^#.O3vSEVkJMZ3x:.0a:v>[5=<%+Jc>#>Z'=1[m.-#*kWD<SIfv$'Z2u:DeP&#Ea=?-^X+020I0H+L@i;$F/e-)jc@L(9Dvs-U')oA"
        "l:V>n7EfX-5AXtt128s7%]Np@H;&rZibdfQx=#VDajBrDwpO)Ra$###40(3v:UD8$35i$#Dg'X$-rrB#%f^F*:D.&4PS>c4LllY#_lqd4PHu/4PEoY@lidA#BpXpN?OVXS+%9^]@TT(>"
        "f3^w9,lUw9c,^x9)S39/L;m]#XeRs$<w9a#b5tx9OfgwS2<=f<ex+BJI^`I4CT:3#/A&2BAh[D<fa8s$Qt1S[CM$T.e?d84qMr.2EDD/(IKU#5xuliT`=r:m(U1i:ZC0Zd,Z@crWGf--"
        ",$j--xh:.-]=op9U[Q:v:f(m/GR7G`gv,)W.K><-5e%Y-0T;R*i;SY,xDp_OVes`#&/>>#s####:$cJ(pG1l0<H7g)lX^q#LM=vLgYF%t*r0o4f_Fk4dR08eb;xv$;w$Jh=2hpBSP:I;"
        "bd?>QiZ@s6TGcY#D@7w#,M1v#H;[S%N_Iw#Deap%;HY##@]a5A2kaJ2YBjNXTJY##>Aur$<,%W?i,Sv$L1Ke69us]7_5(?-pvPn&?e5l$F)_8.6%U+*vEQ5Aw3O2(;_Sq.m_G)4@reAe"
        "E[cHMQ2qd8Cu0^u,ZcYGU*#XP1o]g%>1(vL*NArQC:r;-#WjZ5gKuuuSv7T$N(A-#-Mc##=(V$#U3=&#VrjpLDD,X$<u6s$B7Q_%C<Tq%F[s5&qFe6MSmwv&@mDA+7c<5&D8A-#eirS&"
        "t'r-M2vFT%+/###5U%x,[bY_fp-VKCpo<Q)h21:%eJc;-KfU=N1g-dddJ]o[3Un?uL*lfLwX/0M0TS7RQ=Ii&MHMUMqCn9M`GO8;*BT9V4.K1pJ@Y`=$R>r^rGhd%@An.;kK9Z-fEi-M"
        "JJ5t*07dV##&vOSZ-Z4`eWYO-#,`2+3X-Z-2u9@%_^6Z-[[$AIbQr(Ee5#F?JGiL.$(WUmq(5^>R(M[abN^TR4SG1'a#=C#^Um(<887)WYt`i0tQFN3XO9MB`MF&#IWxr')aRh(tYLl0"
        "#E`9&_l_L#tm/M##Ox[Xx2ff:H4QE#e6U5R:'u)#(jDE-jjpM5`#(BRTHEY0*6(2K3cUi#Uhg-O,_e%+kxN6aBA$kk_mAU&`/;W/rnUf:rwUJ:2w2Q(PGuY#Bh9L(Xa+Ok[G?T.qtDGV"
        "bNA'.W?AjMSG$##^1j%vqOKsL4-f+%rBB6&+a&:'ph:J:'dtoA*KW%$aL.@u)j*#,&V6G`i:_3$;)AnLtcZ2OSmk0p>C_5N@PlcM2sJ%#WNKFOW>#9-QO3Zox<$'8JgxVM.xPr#V+'dO"
        "x<9o@7Ymv%I(UxtYIS_%54&/1IkSw#'W&:'($(KECF5gLC'VH-'fG<-IV4[OM.(1p&6PwL0FsbM;&*ulD%sM0g*K1p^ZaG;=wZi9l/TpA4<XGRV%.A-></A-01I]-$N=kF7V%0)H9/1C"
        "Ee?]/JoAS7<qvM9upMv$henY%%1('$2eJp.rI_@,6YNg:Z6nu#7V32pk*^o@e?1q/JS1v#LP(Z#>;oL(iD;#5J<#=$';P>#rEUv%;7[s$bFd>-tH5F#uEg*#-Jl>#@V4.#*Yqr$*ukt9"
        "i[H_#4v@-#G<X]#8#*9/ZH:&4uu=Z,:%60)`GUv-@Tsv6K]B.*a6,A'tWFx#.//^$'AC8.DlE<%HDE@Z$X,W74b#e/B2DZ^<Q=Y/UU^J5;Ag1;tj_kPt<xX/h_LWInBTOG*X]t7cLjJN"
        "Ss@YS2F'^#'D24BTMF]H/w;3po$96MP%sc<kr]V6?n6T*:2:e4Uh9g+B)vN:TVD`5l5ts/&>uu#XaTj9@o+98ZB^s$PnqjtS$@[%0YL;$*DcY#H%79%&&###[*8F%U,<PA+/>>#P#=8;"
        "'JJL(b`A^+/3=jBBHo/1R4G)4<q(a4ud[L#X'bx#j,BkLG:4Mp&;I/B`^/g_QkBZ]+$s3RbB#aubI.F%^[T&#*w[m%jm&%#Uqn%#fKb&#G3_X'+4%g:,85##E6VK(:uh;$>%Vv#/]Cv#"
        "lg[-)(L3t_H5YY#Hj&w5=4VW'38>##?wXC->OZc*d-/%#1#u>-PNJ&,vhk,2,cKF*$V'f)I^(,)j]7L(=>2h)vCR$$,CRP/_2I?]F80i)bQhc)sxxiL.+;T.#EKqug82I0+?KDP&UdV-"
        "#/(njPEK(ax,1/u3Z]-uISBdMM1h=u1;6J9Q42'oXNclJL62vQS$ug01k=6#W-lU$OTXgLBG'W7u04(&U^xY?/aJc$Mn/&=]G_v-[,tIDx0,=Tu9iCEPn$(#u]AxtnR%L$[^E1#RdQOA"
        "H.6Z$0v;R*&i>;#5<-c$?s@u-$'K0M1D%i$Djo9vj>ae2_wqv#^l2<#L_R%#8A$f*TrZ/Xm2PT%9xh;$;OI@#0Yhk)e/`3;1YF&#A+;J<ULI8%kq)v#rWHD*5f(?#n^A@#wdMa4g'_0%"
        "-2k'X.B2O>$%2M^ZThpLehBiB1rOmu9F`>,')^#5j6g1p?bwo%hbwoQA<l+)]9$crh[e4Rpuie&Gd)&uTs&NaOKPNAmlo%uBLx>-^6i]&@La[=)tZ$$?p_LpB'%^O:(0;6W#c;-)#Km8"
        "b48(#6<O#7T2'j2^iPc4A.ikL2Egt@v>Kv-er0B%mvZr@blM<%9Cn8%$GO.)W,1W-1w2/kbKit6JckV-V1Y)4uUHu@BR[FO^PoJ(H5NnD6o3=$Xdh7V7&2?u2Y['*jK^t$YVY19N*fP%"
        "dN&jF//r]tEx2Z6v3e>G?m*j1$h$.2n+EWLU8YYuc)s</;es22kFsM0$),##o=?$$gc:b945^t%XLt'4$/45$XIsvQ;$I]-S0Gk4YJxn4S(Fk4dc/&G]gDB#:g=20r;wn4$8Es7f^>O4"
        "n3L&#XoRV[/&mM(:`k>$['@]-k%CkFA@n]F:77s$?PU`<2ho5#t0L1<l.UB#?pxl/LK8J_34qm9tTwl/tsNfLWEBc.M_4;dEt,hLEd>t91ou8/.OAw8VYGc*ii4gLaWY*$9qIf$c]o.C"
        "p:_G;&>uu#c2mx49S,N1VoRfLXC-##>T(v#4ac<-thuc'l2K90Rs]G;$_=Snvh8wUPJ@wUI;$$d64]on:77s$AU)3)S=Q:v73Mk+t<dk+`xtl+ES.S/#),##S6VS[sG5>d.dP#d@V`on"
        "/pOkF4:k30_K=VmVDY8%W%?,HN.r%M2J8bM;QpgQg@?>#EW1h+Wxd'=wVe&e,v-&4s)'(%plg2a@*xHnOoAAWIJ%:EDI8F%9*2)*bD,8?ppk&#+d-8@%_-m^=xj+MndGt$-rrB#Yq[P/"
        "?v[h(P(mY#irxF4u5Z&4#]YONSAn7a4%._UVOH*BCJfOoAA,SmbjgOo(_4Ntv`U9VHUhs^wG#W.&.d,44]U;$x5x>#,G:;$<G4.#+>Y>#>P4.#)>P>#Zpmm33Z]Y#xgY+#GQk&#+&oL("
        "c'Xp%,3)m*2#-*?V<5V/doxF4IfhP)S?t20?RERN4MF6'CH?04L=RdHd1%QK]Wi(#vBQ50MMhBc(Dpr-p:k22A;P>#0BJ*RL8,##$I]f.*Jc>#Fh)R51jhV?.cCv#6ZU;$QgZM(qlI1("
        "$m_l0(JeK+GD0i)vlN.)i@h8.EUnLMH5#;/m2j-M.#420,BMY';([W_A9#-M`n8V:c6(N02a^)It)Ew$,(m$%9OZ_Jx<CkE((sduF>BW2HP8$7d08IHEqOZKB3pS96l)M-jnd1%wLdl/"
        "xnoKs`_d>#%[ND<hANv%FQ9s1u?p=Be[%^/Ggt;.q'%,;S>kGMRAw]-=';b7v<hX/.t^C-wnTn$incS7*+'Q/<Xi*`qAEZQD(W'5T'fJ18pXV$E=.H+1+c&#@W_S@,n?['^`UV$PX[8%"
        "C[uM(6otM(taR*R>=CaQE*8hGA?7A+rH:5fLxKS.:f/R3#;XcM_IWC8S@5^#seEF.<mSF;c.m?BP)>>#D@uu#s(A-#NmKu=vJlR*paZ`**>2Z#'/>>#q,>>#iUC_%-<?s(S&<a*.-lA#"
        "eHp:dFC7/1Wf$2'b3L2Bj4Q%=+:Duu49(R#^i%p$Cn/k$UQl]oK.r%M2J8bMm`=q*=NDL1F9iIhST5mJ_D=Gim0ax@66L1p9>q;-<r.>-E#/>-$/CS/Aa]@k'EBvG-@u+jEFlA#`q.>-"
        "S#/>-&)(8/FaUv#]=JnL=vERQ,6KnL#[sMMwr37QL1uo7ixK^#bk^:#i%O&#/S0gL8-i:#$),##3?4,%M/nx49R7Q8wt6w-nk0F*is':%R]Fo$gPh0::cg/)Yr:/:L[Hk(SS<6NSR1$$"
        "2$ASUXYjqI1JN9`CNNjL%#R3%OJlB$jF%n&4k(ipo3C)NZ@lr-0Gvu#K8S#$bSr>(RH$M(VU[8%64vr-DTqc)dWX=$]v,W.;?7f3IU,H3vugiuqU>iF*pESR79e$r*+4X#rOvxt^A1ig"
        "OC/igYH&`S=f&2BnDhlAr(-3##)P:viKoEINFjEI*&AMpoNDC#k_Pj#%;>##9qVV78M058<ctA#5).O4Y8q;-4rcG'[0av#+KCB#s.>#7HX)(XHWaCGHMe%7Q=/_ubsu@/%/5##@il/$"
        "K`@Y/M?8W7mwrZ#g(hrq+7,,Mo=J1*/Wq>$=str10s7^.;)'J3AmWi%/[9a66RZ**1/oA#.6`T.BCQbWuYYG-==@e->dIXLSUu#$ijY3%/.'[NvA?>#kJY*IH<lu>xUbA#lTjDN3S7DE"
        ".bLa4-AA)*DZgK1r;UA72@ZE*)9%J42M/UMf($IQoKx3MnIMx5W9McuX'VHON`k/MPo-7$X;7I-uHRe-^^r#%cncWM'=PcMb/GS7#J>V7Q9mi(1@NWN3Deq7K=j&-UAm6@aB'T(Y=.R<"
        "U3D64FOdbW21;]$L74G>:$Ei2:gS]F'OT%'jBH'#1$(3$:Xq7%8OWR%#tZJ;0dTW-hBC_&#[bJs8=X4'6GDo?Z[.v&`b*O=Y89V--V3B-'5H`-Ds`m:fgHT-Aq.>-)T>lMl)2hL-@xv&"
        ".+w;N#xvl.518a65sUAMPj$mSHMR`6l$M&2H####v[V4#M57<$*5+gL(LU2^;]c>#RbEt.bovC#m4OcMpv`cD*NVO#/5>+r87lduR_hpLHgF,8?BEK#jf_.Co8cC&3SUp&a+]fLFSH>#"
        "rXR@#/65+%:M=+r?k@du<]2;Me@0dux^1,;W*#au]T*MB5M$qNa<Hl.d,_+$I>6L-ccfa$tmV$$e,*]X73`du5WoMMdVb`uh5RX.^H')3$h8?0IW;(s7@Vcur+&RER*oMMS1c)#qBP'$"
        "R*hb$A7>##Z_wv7%>-o'2`Cv#/,<PA&&###sFse+fG0E>x(doLL=?>#Jb's.x'i_?2wA'd[U)v#`Xjf1*;clgCw7Pg[^hn#PD@FMj:6L-o$d<-Nt+G-lioF-D_&P#:SSF-(hjN&T'XtC"
        "OA###'TGs-GcB0Maq^M'ebhfM^%%##tRa%XN>Cp%_L;s$(Hhr?2ho5#69h][m2B&O@-N:Q35`oI&.t]4R'-Au:@cA#3UAl+@kdl+c),##Xfe)$0X9:#8TZ^77;G>#*GcY#1SLO)/%VJ:"
        "bE9d*FiNC46<,##YrNC$,PuY#5KcY#]S5)<n`/.M`RdY#p<l-P]lr]7S#sx+*2/P'O'bT%I<XA#['2%%>@n5/?;Rv$vid;%U1(Q;^:$.4kDsI3pnF`#Q#dC7j_$<0sw*v#D=3gNlIe^?"
        "GHsRX:Vp%,j3Ce;6IN0uJB+qF56P2:/ZLDEg3E0Y#)P:v&MeV?B%K2BT?hSe9[h^u(r1$#9nj9%S&(N$b*Z<%<jrr9rMMCXE8(<-`cQ(#$),##?gi8$?;4&#[>cX7nb*9%CsMA+';cY#"
        "e*TO&L9A>#l-5'$k&&c4rIPc*`3x<$lrqB#&S'f)lk2Q/w7WT/2JdH#BBiI3>5oL(-#h&@uu+[5Z@%=7ZHFHG#'>a4Dkvu6G3sR1xIO:I2Lp@u1?'G5eoNkDP?G/([,ds-@n.T$/@n0#"
        "aDlX7fo(v#C.-##>bx'-Fu:;$rswu#U(ffL-N$##:1LcD+PuY#A40O0nZXp/v3As$MZEYPWRjYP2QcG2T(T]uZ@D9.%g-iKYRP^#8DxC3Mt3>dkO#_#jjWV$#XbD3Qk-<%`/;W/O`BgL"
        "XOvu#;Cv_A9lt&#547W$8qeU.BUIs$0b:25&J(W-t5k]F*>FAO$LSrm)jehg8Pc1ue,HD-/f#p10f68%dOf+M&Tma<oIVkL.&:B#HiUS%:CwS%]:;8%81@W$ZduY#/`C;$C@UGD(2###"
        "/WXV$D_-&/=I&f+G1k63vD*LYhAr]7Uw%>G#fb?Ks87L(^GW&+IBQZ-@F*?#>?1-/(1M8.CpX9`]mPi7^&4fUg#rZ#>vw;b&XEhcB_cSu=5Y6u%u1?u.K9:#ZJKW/fW&%#F/sCGbhrS&"
        "*5bJ;Mqcn0[c*E**$4Q/%a5J*3Z_j0O^&*r=Q>[FvMvnr4tEW-Tc7u(iLlZFtUl##U]2@$dwd(#`j9'#R+^*#+[%-#uXN1#VfwP5`Qj)+FLw8%_bMc+:SET@5jUD?ZnET%1.jeH]v;m&"
        "uk+7DBenW$,^,##iF8:)>(vY#q_oq)HR7s$wt47*81`v#c.//L9XJ2'dCSNKK*#n&okm?#J9G3'8/@8%7XN9%sG_w'lU53BZFPX6,v=G>2I+^6L7(f)-<v9MfWqDI/d94EJM*>A<h+#T"
        "%;Bf3DJY?6Ea#o/w8C6&LpLG),?(l0U>_mCqfL:%D(WT//[v)4FO7lL3=hF*vZboVdoFc4VkKZ-?DXI)]BFg$cf)T/xL,G4@2R=H,CY)4)>QDO(IVs-'Sf_4-eG@.Lt[s$G[*a4/uME%"
        "Dn;H*tMG)%FO5gLx^qV%GBi^#lME2>;mw_>TF623EIY%R:QF?-ZR&F6P2m8/*Db**+N7f)8d^r.gpjv6?u)<-$UB/2m@+N1wl0s-mGl>-Fx,B(9R0t87:nXl]Jt>,-p28&M;L$-E/-w-"
        ".lCG.AL`%,:5jo&B9fA,Cp_A415dP2LoU_.2q(KD6GSj0ULUbuavV?-MO*-*xV_>$CD=M(81H_+;UZ;-:BK7/J6P50t8v<.s_e[-R@TF%?eZL6BPwN';5<MiNj%i;.S#_+M1Cf)Y#_*<"
        "Ycl?,M=kJ2.b3u-Op3,#s1@cV7s:j'<xd'&]FNP&8EX#7*N9S%^Y(v#-cLv#@>5?#5u$W$:H6P&c:.<$/lQS%K-B6&4V*F&l;<a*0<lm/=@t@$pSY/$?m1p.)KbfQ,6o-$HO3,Ra?`YP"
        "'04m'Nu.Q::xhv#'/,##;uk+D3DP##7%i;$H5@8%8sLv>1nV<%9#ur$6)9u&V?[E.Yv(k'.r.=-7f;O=4YbA#XkcB#sTO2$?AoP#nM>lODuvo*6Q:N0>.rv#-GcY#>m4J_h%3Y&Xru#u"
        ">IQ.Oq&8;%U(;W-3(j&-na&%#pCMY&=IJ^-rGY9%=+Is$6=RW$Hcl?#Ehap%*2.Q/>#/-#69'E<Ewr2Bjl4m'geVW-1XSoqOHC=%=@,SW#C;)N4r8`(u7f2;ke)<-^De>%hts5&3o_;$"
        "b9]b%XPaPA*63Z$UBLf/w5:5M;LU58K#-E5u<;Y&k[`fQ<Mfo7>O$Z?2LNd4Gw?68eSbQ'CFR8%8sLv>3hD<%<&5?#xRBp7gcanWi/m/EYT;rZr/@>M.%P%&XaGAT8vHQ8)tIp&<&*EG"
        "r]]=M_xM#nBIrF88hekXb`GeQ,N;JQio*vLE4$##Zg1u(qFp.C*^Cw#Xn3c%jN5N'u[[m/Rk[w#Ehap%0Vw20H(?\?&=eNT%'57Bng$We$8_8>-rGM^'DP6TT;g'X'N]')#eP.;6FTF;H"
        "qr),)v5#,2T&t+;+M-TBe'nNXt`2/L=F/2'*AG##NFRt$?.[8%8CI<$GoQP&?'BQ&C&#v#,^_V?*GY>#3E(v#,GSL2@oXI)`^Gq9+['?7_Yg+4Pi(%6vwk/2ZTa`+=HF:.J?(,)MHXt$"
        "Bp()3m]WI)#0Tv-h?7f3G=<u-Qxun]U45d31<I?]AW)pJ6-Ld2Pu29/vBo8%ecMD3.Rx[#ESB.S%4-mb_YK.S$:H2c;QjqFAfN)I0:$PajngE4_#l-$6c&4Nd<HM?\?D>G>Kp9#&3<]L<"
        "$a]A4$+2i5%J'iRrxRJcOfh1IO%O)IU?-u33vxF4-@WINU[du#3pT3<$7-R:.W_M)a4a2DIL*p%@[Ep%hI1m(`.%W$Aa($#@[WT%'e8X-q<b#70Z?2;;AJq9-_'?71<^=-F*]&6(AC8."
        "d1NT/-vgc$kLet7<s%t%B=&H>+Qgq74IaPg^:<nLTM67C+f%##>:QpKEOA'-4fju5cB?d=08YY#dwV<$6S1v#]I;8%?Ln8%^xp>#5x$s$EIL,D.MG>#9,qo%9p@-#*ws%=8cXI)?mWc4"
        "@9712.K9o8ja-HFAt-q706C#$aW$i1f<Ss$a%NT/DWLD3XXNu7YE3v6*L3W-2imb%h/:5)Hxt4D<1Na<jdYUK+80L(iw9M8Ww#,amTZN:QL<wI%5YY#vC3T%oOEM0dw=PAe3P9/x68W$"
        "67es$TtDZ#RV@P'G)EA+(GF.)[K0U%'U[G;:@Ep%/cUV$B,g>#(;G>#=xt+D.VlY#5p98%6g.-#(eED<%P$##<KjE4`?Y-?7-rN()`g'+$^Ul0fXB+*:>`:%CZ/i)k/*E*_Qo8%b$g:/"
        "Zk_a4DY-.1NJ>c4$=_C4.ab1)rSw&4n`gp8$$F0Wu[LXkDHk?:YnltA9&.F%*T6v$<GWm/2beNV3<MK2./^#B+VRH$9td>,qmeNVc&Tj<8UeNVi=YwjBH'@:E^Kd3^?oMVBCR@,%i0ig"
        "8SJWK8f+9LXIVS%]K-)*j>glSvI]`*w9,GMJBDZ##C@G;$HZa<:2E'#EpPEnlA#v#bGlb%P-ngujoqr$>BG>#*Blu>+5G>#5gf>##[a`<,.;0M)Fo8%sT9@1ZQKd+Wu,##IZ0V.Fk#OK"
        "Ikk&#_gc7&``vm/NIHg))wZ)4jsJ(%a17H+c2T`?tG]KHqvQjaMD6k>S38B$=PS(>.QO/YShSUgQ9u[(0e/)QOK<=q-h9SI`5;#5jTvu,CBWf:]'x>#E*+A#4f1?#$<WK;/%Is$Z%J/L"
        "^a[v6<4:,D.`(?#1fqV$?Z:Z#/NM,#=Y/2B-`:Z#MHK88B@IV8@Gs0(x]Ul0C99:%$C>V/fR'f)g@h8.bL&Z$r]B.*iG)w$Z[+dt/JmrH?wK=?=17<$E<9T/kqs`?PjeNbPr=#/-N9/1"
        "[17<NKr%5f;B@D$L<]_#1BF0D@Zh4=2'$pLOj6G*Y6Ev#uflIUS5&B#:%_B]a;[A4d*el/LEUq%tZ1C$%),##E(,^Z3h&K10g.-#xQED<$aRh(0G(_]<x]fL=RUv-@ImG*Z<V/)Q`.T%"
        "ltF`#ILd8Gx/@p<xKLf</LpF#aLc[R'P^@fW_bEeV*YdGrqC*M9&UHMVHe][^$fNNJK;'#dZ6?.VIr;$t0@G;`cqT22c$s$*DY>#?2p>#1cCv#?Ej;64m.-#(eWD<eWE#-/lK+*rueh("
        "S'XT%uanl/v#kV$fE`;%Y[RvPQ=E.3GP29/dufKY.:M_8(__O1bB?)@6(SF[__;&IEpf-3;6BNMrb[vAV?wM(w%I%A[NW'167>xJfK6v?3()-#%3_s-3)]-#xsCC$m@%%#NXgL(vh:J:"
        "(55##E80##RK(,)m3CDN9^mY#T_mY#+fHqMkuxF4t8^@JPh]1Pvxs2F^24O1jRY=RR0K:vN7cRXG@H>#R6P8E,wHRM&&P5vsdA(M%D<a%YcuY#YUNhuRjn34#MKUuU4gKM'&buL1h/`$"
        "5HY##9^0%#U1FcMYK%gL&59L(V2W&+RgCD3&aCD3u=DSI#b',bVV[Vae:IFVLt?Y31RSM'9HLDWCB',2$&####s4x,46+Q'A>q'&X+V$#XBNJM/7@r[43uQ#TSHo0_Bk(N*R()NkmhtL"
        "H+elLK1+d$'V1_$JHLk&ZW6k$c22Q/,xdS%Oh`f_BUi5')l=@BKvjWhQQOj(`QBDN;EmpL]W?_#,[.G;72Q8I?I,87]tBg<2.3v#Me%s$[+>p'6xUv#&23p%4%`v#DJ:8%*n@=%=/>;$"
        ")Gc>#=p-W$ef@gLjxJw63lkI3A&')5_U[=7m]_v-,G=.)jWt(3jGup%Bp()3n`WI)q;Tv-X]%>.$ed>5'h%J3>5oL(AcWF3(ik'4ULD8.$l2Q/58baG2/MM4>N_^#SnkZN6^4RKp^qG5"
        "YWq,):'@.FoD,a_eN.>/SA>g3ROjcYv23GL$L.&+o1Ow8AfCr0;%N8I>36EqNo;l]mm[;B`2?)6EgLL#trZM1[rXIG_53)*kXTb`bA_.G'(mILOndY_+5e]#ukJfLS2_^##PR4os,pVH"
        "xA>G2D8Qs8.JlY#EF)v#j2T[#&CRG;+Pl>#=x?VU*OP,MEo2g;3(r;$7a_;$aeYF#Qw8U/0,]L(W<tT%NZ0B#o8w)*j8.$pCv19/jQ4g)3SAZ-X?Rq72.5d3'G5-=cce`*8]>T%m2pb4"
        "O+22W'd7+F.*w$8]9:/1Xm<OWW?c.AP0(rDKThrHDCBe6DrxaZ-Wwf5L=*)5?T%jDr>L=lV*iYIUXajccm`cWv@uG@br<EQ07Fn<8t]xth44R-#m1U5ghc+#xTZ^7Cxk@$-DP>#RJuY#"
        "Ytvi(=$A3kWVcY#A%Mv#?\?=0l_JG>#7VTB%3)###Hdb?#U5*a*Yo&E326tA@m'nL<2XX#7KT[L2K3&&,_R9Q9+;m9%ad0a4IG9V-r-$Q'8@XUg1`77&ZC`W-O&q&-SVIdFZFrs-/WO18"
        "C+bYH_IGm^>.bGY-%.B68w=Sn?)ZY-Jph,=B&Y%&+tHa7GabW-d'k/Eb$@>#%=4V?<gM2BXR-5/<48W$+Gc>#2tcL1Uc'##2,+*$a/)#5T-F_&n$KM'Vq<p%#EQJ(6m3jL+`Nj1p:gF4"
        "?PjV?4:jpWq$Dfl+Jd/tuIN2K%?Bx#-WM[]&C$tEg-Uj0OQV.hvDSci)bX`3F.S]4F1[>#9-)m1_O%W$E3FA#xBRG;(>YY#85f-#SjL7B(&###][YF#+?&a+J`Y##<8/]$.x;X-2fR=H"
        ",=m8/oiOjLA&wLM-=b%$$.0T%q9]+4P8`^#B0(F8(Sa/>vm>HEpm<DAbO0ji%b_+8/xA0>+<HHEt;9&BDE_2DdC[h206E%Bl5Q8I+3B>0b(5Q1F*nD4HVjRL&IdX9`'F/37]tK;[YP,4"
        "$':FIGgM60]`N5MX;fJ/&`SV8A9#]@`^O2MSP?$$XLG%tx)NJhQiou,mZq.C.OAw86c*B#+AVX.bd0d24xUv##B,##8@Bk$blC;$2%r;$BAur$3+7s$Ut&<(Q7lM(jZp%4.xJq/pba]-"
        "P(-c*WO8<---YfcwtTb*_I#40*)TF4>U^:/S::ga<X%u-je;j9`#@dDEQ)hKBr3MZa:osq,cbeAe<KJQ`5e2aBa%14^/+u/G.s->hFi88O)3cP&5Gg$2?O7ncJO7n:hJA+BtJA+V_n[p"
        "iZo;PijCJQT1Z`/<YC&,0iYwKDlk^.U,B*K&M=;$aps@kxK%3)=:a'#wK)%]h.`Z7]%&s$+0@@%2rUv#]u./L94NP&Z<1d2A_S2'9@7<$H]qo%7OEp%WpbweY3:O1[3P,2:BEe30@#R0"
        "m4<9/hRS21X7D<-DC+s%2r3*<Hq9T70CYI)kTW:.PHF:..OHg)I5^+4>/&,PUcUemWe;>O>9`.HfSDX^)%lvDu^o7s06KYMjEAcZn-XY7qgKFnhKwW%^v`YO#Qo3H09>iF-7Uo`R8Mf7"
        "H:D&j4s2%?']MF=$),##BN@D$g'A-#g1%Z727xs$+AY>#tD(v#^1,(#VNlY#Z2Yb%5iUv#]8(5McEWv#6T(v#&_G+#=ke%#&hn0#5/fL(%jhl0^J_#$#Gkh%hO`v#8u0N(=[%?5JP+.)"
        ")f/8J=GlY#'a^-H>$YJ([>VHK)-g^D2e`,umWvA2pv'i^YZ8OC-Qd_#$:Fe#RabOf;8j?BR/5##jat*$eU9:#`ja.DFUF&#v09e%G9tW-sWgbaqprs%:1r?#=K$HF[[wl)^^D.3>U^:/"
        "*^7]Rr]W2>rm(?AhDBHF3jsgEd@Qb:[QJ-PTCVuC(ag?ISb,[ASUpZAbAiu5/mgVHcn'&+qdai0@vp;8?clY#:JLZ-X4%s$nQ$iL4%TG;%oUf:2lCJ:(Dc>#*>YY#=Ja-2SuiT/Gabu-"
        "+CJs$*8Gc4`/PW-$l]OT2AiT/S)Qv$jUKF*X:bBKu@#G4i6Z02:M3q;J1&g@TwAUJUi5C&$%[O?=cgC#>,Fx3p:dXAPG.Vp;,Or;t4XT2%0]jLI.T^f32]T%XU%d>e3BN<e*X_u>nJbu"
        "0+$DQ2I&m&Q3p(W%*xu#9J5N)',,##T`'##u^n-$W[?##Ce3p%xDQJ(Qrn(W]&Fhs+-f%#woxXuv+A9$](k1#sNV,#$ULS7Cob2_bfLSItSv9)9gM:)>FP'v8FI#M$TeZ$:u9hLO=AW$"
        "h7DmLRGU[-r5>FI3rS`t6cPhL>c;bV+f-s$<7&@'PfScr;WYF#ODB2#-Mc##@.`$#B^30(S0tp%*2G>#$V:a'r@v5(:x_v#e=cB]MMlY#@O+a+]2h;-=1;t-1^%7N+,FA+u&0YQ%wo`?"
        "&bQ+uc8$3'OKgQ&wPG<-qGEU-t(2o%%Kic)lIo-)(a2LB3]UqSa@ofL$Bc&T$oIi'qcp'aHuu-$47&AZE9%Q8X*wxFkYMfLG((At4_ub#7d/(18l:$#-9rm&C=7W$GY(6'2`Cv#037^#"
        "3E,O-C7ggLVY7fMgLSjLVeKc$dR<p%5Suu#=tat$2K'1Y5A8)Th'u4Xx?\?^#2NY9VoDZ9V[%RS%f='<&:o:Z#mC$L--#vp%:BKp.0,>>#B:t6*@OxI-eT[8&n_PZ$>kww#CtjnNU4[bO"
        "8X5[0'AlY#gxD`#@jdtJXcO&#9v(U/'uv9.$]0TT)vlDSYeOu5A5A7[d$oNOY/5##ilr8$vW9:#eWt&#JFS`,_gtglUlCZ#oeSfL6=TnLnavY#i=[qLqFZY#2G`t-aMrfLhjk&#DmKS."
        ")^D4'K*=A#asZ)*FWc8/F-#9%r[Fb3*^B.*UUI8%-'[.&3VeJ(PDpF4qk;Z#p(DG&i-?TY9[<+e1m.42S=Yn0.q:iG#Rk=8-r'(f;NKfXC*Qvj*BjluG4P._-PTL(.Oh>$]#$&MP3<_S"
        "2W*C#.F)##qwqJ:%I7G;.AS/1Gi_V$[c3($&,,##rJm-..>_nL.Tb`#:&Mg*m$,C/+pQmMN;r]7#_>@.JTUK(*Wv5/$gUl0l06g)4o0N(V$vM(xc>A=sTAku*D@`$n4boP$S/,MngGlL"
        "S[_%OEF'%#xaZ/..9`#.1`hV$-V(Z#CJG?#fHPi;ZvN+.Zre+M/&a;$uOse+=[^>-Wlh0,I2>>#n)`@-EmF?-?<5Z-.C)+.97t]7cjE+.bo]b*?Ads-2a&,Nc-I:Oo?@>NhasmPb93iu"
        "_`5aPXuPM#v:3C6O#h#vNccf$bf[%#1t^]7tF$,*6+7W$=AY>#upW7%lHeY#Ti%/L3oQS%/VY##BAur$-o6s$8m]Y#1aUV?+YL;$5N(Z#XP4D+`3IA,ETFY-asY3'B:7W$VV*&+S-V#5"
        "-O@X->cWF3K50J3u;Tv-RqUN(G=<u-v#uU%b8`^#IBH,Lam)74t=ULc3Dl:o[*F#OeGm&odi7f#H.0h$rpldKbZQCsDc%C@xZ.&fv6)iKqw0+>xdBlo$`$##7cGlLY%buL'WE$#t8q'#"
        "@uH@^B1[s$R0Jmq(&)t.+A>>#HIUcD<+Rs$=]hW$5YG##BcB:'81VZ#:auY#D3O]#Kkq80Sp&T&Tb0o-=tft1]8G?#cTs]O,LQZ?>_$121avN'0Xu-3;?RL2reFj1P6At-3n2D+tSMk'"
        ">U/2'6NbH;v8S(6t19f3R53K(5e<^,pZw9.XR=F3.[v)49I$X&MBde)]a@AO#ab_GkmhP3oem?j;C<T%^l*CiB:M?#dc>n0HhP31N,'Y$2X,&Mn@mpdNc,2+[1;2Ta6P7Wl[L_$BsuZ&"
        "Z^(h#(1XUWF4Th#(`9(N<$Cuue[/E#x]j-$8@$(#s.=Z%@3Y@7+a`f<C@q-k_,2N$kYVT%E),##w&DK$U6K2#j,`3;>jVC?),Q>#%,,##BE(v#VvboLfntlA,AY>#'P>V7Nw@Y-RQD5+"
        "@u^s&,?19.+tZ0)/6)N0-O&5]O.kPt$=2+Mop9^#68V4o/o()jbhho.TC+N;9>G##lOvu#81`?#xBRG;cJ;oCVqv##Gs[fLuxQ>#N$###.P:;$@T(v#-^_V?YG;,>533D+KjTr.FW'7/"
        "AVQ-H0'Go/2;G>%073Q/caSi)43V,)&Bf4(vBo8%bH]@#:W9%Y#LHa3eJl<Y$*4ult&<osU)W60/Z%W;Gg%@X%o@VM$Cs@?adcV$0/bOE`$%meS6.tDB'IR74g2d%H2Puu,KqN$/,D$#"
        "I9F&#4sbL)Cf1Z#-G>##q^%##ZjJI$1NYO-[5Co'bLw8%DME/2:r=,MX2A8%^e)?#+@:`$'K=r7d7m29`r29/,jd_u*wvf1gl@du'*P9UBa5%Cbd&a$ls&mNF67qisdjlAiO^r#&epFV"
        "X5*2>AZmB@h;d^Onq7bL?BebIEJ2kXP)>>#k2wO$4n53#MxP68tZbA#`@)<-,((t%L]1W-1ja=AT#m2$nhGgL-N0X#C98?N]DF?%/u/Y8&1%mL6B<D#o5VoeC).^4.iqr$/4>>#-`H>#"
        "S7QP/]R3)3eQQ_uMe,^4HLLJ#@o3fG#)P:v&HQrd6k;;Q0JmJ)]Lw]5:o.d*1HP%P2gxW8kJBx9_^vA#E[tv.GsK]=ds9x9uxefLcZ;;$-PB&Z`GXV7XX[W$,8>>#?w?k2_*[>#U/$W("
        "[0x<$LBmH3&ddC#q;jV6)0<UeV<3,Sc`p+`LnvFS0EM2B'977$jR@L$>WtEn6FB2K3f[FnFvKW-U'qIn-].sHh%@PpM/SGEj7w1qGNZMCi.[lpNj+B4<Uv[FLj/Z#&@wr$<(ku569=^6"
        "A3rk18,s0(JkNT%94.[#d5G]#u9IG;+S1v#T->>#m;``*)e)U%&(0:/J*&&,`vd)*[+Rs$$`^F*;XR@#-/65/JeD[,Ug'u$iHE:.Dp?['G5m]#Q4q9Th.^o49@l**xhTt6`T^:/xB9lD"
        "MM_`E<[Yx+#h'BuHSGD;,dU6<_&BQNPjO>CPR@'81_C1WJD2K#C67_8l]%J$@DuI>Oe0C#KEoTZ**MU#Epe2HVSm100YKFQAL<fJhhF4$Wcvq]kj^w#>pUo^Wk'9ESNk]ueY;GuIdXb'"
        "C.[0=siFJ(iM>ulRhhfhSc8>,#?#,2Fkco7mIM]=7aViBWqDYGl@F9ri9E2#^?)4#^bd44%;[<.cRDZ#Tvu`#mSb/GVGS/(W]maSWnnUM8gQ@&/q[G;88YY#TiB>#ij8.$/c1?#7%I8%"
        "K;7W$FTc3'GwSQ&Q-JX'O5i0(fwi*%v.<0(U1][#J,Q'=uN0##7l-6Ne0,,H8AQ1FU70?A9pUD?uHW.=N)N99Ains.QkZ/=it5K)lrqB#,W*i(TL.s$;J-W.`?7f3Qp3H*-S>c4h.<9/"
        "<Dn;%U,-<-Lq[U.l&e)*;`><-=']:.aums.E+tY-O]dG*Z5>)4D,-g)I>_/)_Yhw0cGxiLji-]-FpKe$W]Le$;fq?B:^d*3K<Vh5>(*7<mbXF3:n$BO9T`^#-Uqf(]V2>gM;%[UGZt'&"
        "6g6,)-vYg5VFP?%GiZt$7iq#63Tm85&CrB>9jqluL1Nc.%'b8Aa&jW@0_+A#qLK58ZYPu70YCv#85,u@(aOx#r>k2M_dS/NFjmC#tH1oe@E9N)6_YbuN.Kf)V'(;8uPiOfVP36NMDO=u"
        "uS2S08(RC+Vrm2'V&J%/I@Ci5l@k)F)V*t$+0nSAd=<d8]R#A+k:BW]14jfhXEI1pMPtGV%KbDIS3=LUSXf3g%*(VQN@Ft?lunY#&iIs'D/*5AWl?S(rc[hE'SPq*AuF%RGm?k(Hk0RC"
        "/`.R_H21c7Ed#D6gUD6$wITb*fVOTKDl&U#^C-5L&2(G7-q$7_W(@a.s6Z9_utHk(iP/puO]wKWN.J@#NIOt?krR>##VcJ6dihh*R`</2rYjRJ#&>uu@*4A#p006$8@$(#LE<^%6[jK*"
        "0Ov]%J)>>#FdjP$#`R@?Rj@5B#00<-1hIv(DpgD$X0YsLUGxO$*n=WM85kB-Z7kB-OXuT9'4U-QEUn/Q*>6#:7,Nw@cM0,.a$,lPG1>gNoUNrTNAC-'_T#Ab?rlx9n?3x94x2$:L#CT."
        "$2YY#JKtn%b^:Ms_;1[-=3+o3xxUvd$8>;Q13`%O/HBs7nbeumVYW;.^lk'#PfW5v,QMo$;Yt&#C%%`%o.ku5ST%29._%E5,bBZ7e.mc4A?@12wXsT/]51Z-Be2D+&8xh(Y^5n&=47W$"
        "v`gA&)l4.)RsvD*7abUA/WDh)]W^:%^Cn8%b$g:/lb.W-U`c2aajxw#8_-Z$>$uU%MpJF*#:8W$s19f3gaQ)*w1+.)t$P`#Lhx+dfh8gNS&FF7.w]lH#O),9RC644Tb8gNW)hD%K7k54"
        "8I;5/C9%Gu&jCD3o6f^#XS7u.rpT^u`Q`X-Af)I-`#)w_O(_J=%::YK$(V1EN]I>'$KBX((n>X(o6:9M6?*)HbSZC##=U]71?Ha>xLq;A74`m>*TcS.mjdOoDs*7/$Q-`W_Y$<8X3h#v"
        "g]MVd;*',2ENOpBCo*$8dsw(#vZP+#6AOE$^;G##6w`V7R1r;$t'`f:w9(O0QF.s$i_H>#@BrHMf@[Y>rQ>Iq[CkCNXgcA4@iL1TQ<h`*$A7V?0o9kO/xG)#l3ZS[Z1vr$Gp?D*+u'58"
        "S_n>#)/,##<%l+DPvwC#A.u+D5Pc##C[<T%D;ns$4K;,#H>Tf-$;wN'47?c*n7kE*6aRh(bv>R&GT([KgjCwKj@H>#mvQ#5-aWI):2Cv-d0:]$'<1^#*7Dcuo1/tudv@T%al4U#OqGj4"
        "9)O_1_:Slu+AI%%Lv.Pu'J6C#&?*Lu#Mq'u#?Hb4r'f;>s6YY#gBpx4k2W58]G.kX(nYv.(GBRCfP?R&xgp5/TiB>#t%WfLRlgi'*Aav87QR*NrJFb3=:n>#-GG>#tsY]4l?^lov/;Dt"
        "W9@m/fMwbrI9aL#9XN/1aj<hu*InW$qcmatY@vl0#ABb4T88X#]sVCsS^W8&^XT]uQUVa/CR@%#t;+-*]?Q-&>?ovPDPfs$8L<5&kkpA#q[2a/q(.V?DD:;HwqG^#q)D'SAO9g-D5gER"
        "VH[58]YT,.;OwDO:Ptl&:&69.DNXrH5Bx9&Q&U#$uhJt%9Vd8/#Odm'vT'^#url+ud?Gc4=h3SRVT3iKSt9P1Sm6V%9?SY,Hk)sQLKaY#7efi'k2U`3@l?>#'>Y>#<2#?#/i$W$<Gx-#"
        "1chV$/p-W$1T]Y#*<@%%`c@L(NqNt$4rAL(uPCl0t/wiLDBeq78Sn,=?\?n^5=;eY5t$&/U_'3'ut1C&$D$f1$MKRSurAPYufMTqud6RA6%hS'FVU4K0AU$DELj$gL$*%RNlBA&#<Ja)#"
        "56o-#vL<1#M^?f7J@l^6s;5B,E`uY#Z(nlE7DC@%Y_H2PK0fw#.d,##_K9u$W<B:%*>YY#JA@W$RPRl'g^0Y$FbjP&`k>n&LB4A#b/MK(ID7@#8g;,#SoalA7F7@#Q+bbIn=+F?-&Lj2"
        "&p#d$vd[iPalECSv+L5NV@*MF5(f'IJ=loC+RI1<J1;a>+rZc4XC@=7nXF/2^6HDQ[wZ##^xF$,lrqB#m0wq7CE%f3r/Y:.vrse)_g`I3K;T+4FDg`X.h&kMmuEb3(T&S4DwW/)bLlG-"
        "h3*^/U)Ab43tin[R;rpfMj5c*Eo4F*mceZR76bT`QM:N($H0bE<7T`td@U)OQqr/u:LU[-=Qt<1w*Bw4H6N/M;,.Ak5#BT*:#NVSx[oL(6hl*7C82d(.iO]U*H0i^DUc&#1As1BO9*)#"
        "5f1$#[1&?$*Jl>#*Auu#8<cY#(3;,#$h7Q%-nw0#iXRs$IBSX-2H`hLCPFjLG#'%635)5Jh#S%F'6eIhbHu+M[+O2B,]$J$ir+2##)>>#mL+U$<0cQ#&'A-#&B^h7bV3i%Ahd84&4n0#"
        "Guo=Y&6&F.uko8#d7.Z7l](v#vP(v#XN31#o-_)N1i-s$'8YY#>8,?#3ll##0i&K1?(?\?&4.RS%]DXI)ugn0#00k-$hw*t$3lRB0%G3]-[LH>#vHtWUNd=lV*n_U#PYv&MCeEO#-B]lu"
        "#3vW-+t=U)jBqtO3afY#v`xtuc=vtO$nr%F.gu58gBd;#Gp&m$62<)#fbY+#.h7-#.OQ^73B,##Ee&q%VsTR)EO3p%<MP>#COUcD;SlY#M1M;$MLqcDIL[w#hYuoLib93(8po>#5B0&="
        "26snDS+k#A*'p+>b=14;URQ-=(=4^658VZ-YKVhL+nHg)00rA#%u48&[.OF3xF._&X1;jC:7pG3Mgq3*&((f)TI6Y@_w:9/t08W$wkM7BLVbA#CecA#`<%gL*.8W$RY-L,@hYrdkU0q9"
        "AYCBHJq?qM^LKBHK]9W)0A/O4*;$G4;)YwL22*C&duA@m_j>9MEh.Y-KD7:)XP1I$?1bB-hrldO$.kB-3ci].PNdqu@0QD-x'mhL/sJfL`%w_#[0ZY,K&g`jNA<A+FDm89HclY#Ib`Z#"
        "%@IG;GSP>#t;,##(85##:m]Y#.`1Z#eCne`j%IC#lqBf)j>)4'=`Y##Lqs9%-sdT%qfG>#WOFb3^[)W-6%xC#)Yi>$G?Gc42w/MuJnZ=f$+IM9PUAiK$g(5J/-f`j@7H+D+=G%X'7]Y>"
        "=s'r&3#+GuhXZ+M.@c:#EYbV-ZKOG%i?KKs$%2/-PHQrd_8=;Q>Rco76V6LC;AK:^jA,^CVebxuQAfi_7+:4r#wKP:,X:#v[%3D#.A(-%YIxiC25&?>t%c_JfoRU%RgJxtN;M0$4q/*#"
        "1>O5;@pVC?2fr;$:bsp%.2,##@1Q_%'>uu#6T(v#;9o(<w=piLQC6##-_.G.3FU#$.qw?0BL3D<uo-:.v7rB#$Vi8.DctM(4^5g)fi/[5m2gjun%0XUfm+O#l)nVUkY,VH?&GM#Qps7M"
        "Wxj:MT@?>#)F]9VvXW9Vw<.DEL#s`,>%Vv#MTpQ&+5,##FUiw&')###Bc69%_OXGM:[b/MTOH##6Bt%0?OP&#UB:@-$If+/=####0wA*YV3pV%5cPb[QO9=_-n4eUpo5:`j5oObVx')#"
        "JiW9#'`.L$UIY##f[eZ7oMcY#.Yl>#CDb_#+J(v#vV_V$tDcY#6kd`*4nQr.JZLg(hV7kXv*x8%(wA@#povC#-4=/O]'fh*+(k/1sZ^1p'cdju)n%T$0K*UK3joguxZ9J(NTJM#o7YY#"
        "]oe`*t^Ia*k6;T@HMn;%,Mv.>BUi%k$b*g_/%@;Hv5T(NKN$v#:7TuY-*[]][a.7;RPWC?g:E#l&3#&#<Y4A[QRZ>#.jESikDL>AaarS&dJ_BONK;*/9rlNSxK3.$4Q%U%36q_a$NEGD"
        "Z4vr$54RS%FTes$01'o0g(3D#=AY>#N2q_a$ZFl/Rladt@m`J:ittQ#pV+Q#t@i%+1ae[uAdJ(M;r###wl+:vt,cfU'ZSw9q8Xw9OJ>##vhM/)5)4T/%dAgM,iI$$g-k%9$4SRndsbC&"
        "3qjhuEA=V6?qPN#&CmC&t]W#$OBFk=W?gk=he35&Y]^6&#pa#$fTwxO*h;E-FsGmV@%x`%a?)X,spei^7dxu#Kv$)*Bp0A=d9o>#9F7@#Ivq0(5xUv#wGcY#a+]fL9?2,D6Bf#57&5?#"
        "7&?f;:+Rs$=2:8%JpX)<#2+.)^*.&,ANFu-vI'+*dagQ&9Yuu#BK>R&nuqB#QwrV$oX+41;FQ,*XX@8%*G2]-O:pW9^x]?t4_3[u-GQ:mPi7_#FME+/H5B>,.AVo$hem'rIle].eEw7E"
        "AgUn0&4Hx5t7&>EfA.[R(gl><1uYL<d8hnL?6Uxt?)7<$Z'@C[DwS@#pG$&4F`(cEfUC^udRAa#@^tM$Dm_bZm5-murDNt];nA5#([d0^5D2i4n;#&#$s#',N-Ra*Vhs<-Y@'`7.sNVM"
        "/-h=unJ_g-2'F_S)8q&9sL4tVB4)n8()[(##)P:vgug(<x9M;QORfA49TL7#tnnq.l9e(<H6u9)tLJ&4`Mrq)A%fJr&N>+#ds@u-w@6UNq-wu#2X4A4D+k]X0135&_'EM0)8(oi^E]s$"
        "2<'e;F%[$$:Xew#nuqB#p2(:%&aWg1<?v:u$A,#u7w=:$$Fk*$&gkq$YO1,%CF(mup=`S]gP$m#$A<PZDiiQ#fQ/(MioB#$*viOfIm(dV-v:D3FRKV6]>9f30:Gn0,]'f)N=MZ#(wm=."
        ")>YY#q),##Vr^Y#YQ<1#)>>##];[A-AEA?2pZ=9/FC_+*rM:l0/GG>#c'4X-.6Iij6m$&4Jjt;-lV8n$*su@uD&I4?qOtiC>%lE3QgAm/Ek?IFRtWY9r]PkcW6*G;^wNX-GHRliqPVG)"
        "UJZ?\?&IWfivTos-KNOoLQ'[lLU0[##FPj)#un7a5G3M41>*i>?7[9N(C'+A#<>@;.7@#R0P+`jL<ZDj(([8W-&eu1jeND7[9ngs/HtVc1`aw<Z'^0B64nEN&JGc;-):K['L8G>#squi9"
        "(x^>$;6(@-q.</%nc><-8,Me$n9-+:Yw]Q(,fq@kuUwm/uME`sGuqv#NuEv1Ie[%#[iu9'6`C;$bhbgLGrOP&8iU;$v<RG;1]hV$(m(Z#$668/g.K'+3n:hLo7R;%r+rB#Y):0:s?S`t"
        ".j)i-tQ)O2^?=)9%cM'6[[Q:vP@#58B?_JL1D>&FH*;^41&+*vPkIh$-AP##?dS2^4G>##>W%O(bxs/1hl:v#tUr,u*KV0$td;<uvC5>uQ0[Cah>(Pu%<ZV##?kxu-pR-#0ZC;$Oo,D-"
        "<b@Y/2tTC*s#GJUxW:m9WfOWJ6_YCN5Wg]=a.wi'Yc'##EDswR:(Kl[[=KfLQQT%#QYJA$JF)w$[,4.#PD)w$`fPlLZ<AW$)ZLxLFT=Z-pE?_8oEiigM;s`NM:@W6wlo=#'?qiK1#65&"
        ";jv,&,.'o00C4D#$X>r%sH_`NY)j%$]?h77=[k1pi^Mn61k[DZ+ilguPu14%Iv:e?W--%+?[s]5l=mM(aWdA#h8];%Vk.W-`:_x9o?-a*<b];Qp4?[/NWqE#ODL3$SOn<$)?TR.tO&xX"
        "G*6T%m^r.C3(EmCMrhv#9Vs5&Y*0s$@D:[#/R[G;0135&KtQdD2%78%Oq6dD2uqr$Qn-dD0VYY#W[x=BtmW?#GapY$*H8MB1_nP;9*L?7Vp[l1-#C_%K:SY>dt9VC*F/[#(G,s-^HuBG"
        "S=Wv6SOr_,W>uD4t3hfLk=JL(?vp8.J>#G4B$RW%qji.PVR?i;0S]`=O#A3D8(ft7?(Dg,@TJ5;Y1in1M^[Q'HG4q0)',oOfGmgNo:(W9aqBlWI1C^+uN<jV+XE]H;3q_6Cx.&6siIb%"
        "k,Ch*nP8O471Ys%.)1s$'ERl$]b$##$4(rZ]c65J-f%)3Z(3[[3lj]7.9T[#Akn@#eNn],+Guu#IUx[#L,f(+7.Ee+Gh$Z$E`hY?1EZb+(J:;$<IqcD)Auu#W)uoLWkvu#8T(v#=_8f+"
        "'kwL,O]Lv#'K+C/:CG&#R3t[?wOBO1ic-<.<;Rv$p%rB#`^D.3bXFd2lxw:/S;[]4MpKB#QGg+4^2M#R&(F6JK#Wm/>_.gD(M5L<U_v.FpWQ70m[;h3bE/[uPuJ;-*lYK#lq/u.63TWA"
        "@h@JMqrZN:.?i[#MI3E6<maJ<AHsI3tn#g)rB$IuvJn9Ef8%L2NieE3hcX)>*[b;.hB:(#-&8N0N03L#rGN-$r2h'##xfC#Am)a*AVUV$#6x>#,;G>#A@UcD0DP##Y8uoLVeQ##20,##"
        "QN+oLiXOjLx_EW%u4rB#xlFY&b)^68Z0h#$j4s%>X=.T.Y:xRC;w`N(f=5L<L]$03Qb;v#EuHq$T%,gsuQ+U%RTA>#0xIJ(lKdigHQ%v,/5jb<g?E]5gVuY#8J,##vW:hLElSG;AF*p%"
        "3PrZ#uCYr7JJA+.P#>,.9X8L,(l>]O7qo`?kN7V/FL*p%1MlY#BU_cDOE>N')2G>#aN+(&ZA&2B5`:v#wZs%=nE7310VL;$a8:5M+`;;$x.2/U:K3Q1fae;%qs5D-k_hlLZYm>#;Hv20"
        "9E%?.&1DG>_714;6-qv7^JBf3/7,R0],l>-4xT+*.hkr?%B(l0Yf2KE=cR_#NwugL-xu8.#t`a4_'/n8R*&^d^ZF5&e)VP:@P0/),qud2:?psA.,Z4<wHL1;5v>G>q74(&:pNO:g:fwP"
        "o$2J3,]3f>+@;4;_.MT.,O`O;pEkn/);4E4;Y1w,<L:*'VM7*3x=+T^uQ+S9nE4F5&9sG<K_P<8N`P2r(Jg;-XIVS%4GP.:^^toIv=ZS%jSMY5J@e>#v6@G;+S1v#U05##PW*F.*<F;$"
        "6>x-#5uh;$-a_;$9px>#+naD<J6gm&[M@L(^Kt9%t=R$$&3lA#&aCD3:H7g)C2?nt_Y5juEF*5J0J9>$2^WcQk.:P/aK+oIs)*O=N^AxtWdGR$Z[<1#?radtd`IU.AWf^dL2hl3'b8I$"
        "B<3)#4xL$#pH6q&IkNT%Udm;^^Rd>#p4cH<mV7L(YJ8^+>#.MNcQA8%MpKB#tJu`<$&$a<.gQrdqoN)3.qUo7.aTCS/87eG$h7]Fc,usMUAn=Nq.w##T:xr%=47W$hg@&I$2s0(spZIM"
        "0MYGM2kVa-l(K9_p55GOsRx>G$nRxFq(_4.S2CSIYYk]>KG#7A)>BjaP=Q:vj`6R30#mlSW4ar?*Z/m0xC$/Mv->oPPY/$Rdn-]%O.7>dXop`=37gA,u:/>%57BY8;u'e=;i1p/#,UnA"
        "_QXw93qT_J.E12MHPChPu*3#&'AK315mr8$HX_/#:YHG$EYMm$G&>uuQepxpKcT[/^j<$):*s?Hbvi%t]tsi'&v.mA)]/'v^qgMB=p0W-K6i--B&oMB*aYlpuf6Q8JwpOf.ONYltx),)"
        "1c/;6wfOv@m@o$$'C@G;?1I8%5.R8%X(ZV$3oh;$WaC;$.V(Z#M*,##1Y(v#DZcY#)TUV?)3(<-:i5$&&/5##/Lwg)a:dl/Siq`5R)Oi2q]:$-;_mg)b=+j$u2[)*oU=j1(e8r8(-dg)"
        "oeZP/%x]w'JLD8.kmqc)vDpuG,0vH3N/;;?Z`E^CZ;4^NG^7EFq^uI=lhQ4N?Ki%O(oM@#5>*T@xa>Z#u3UG-jV_].Erl05x4Er2Z^M#$iEom'Zfu>u&Y?(#rA3`sC?+<-?-66)Ycb2B"
        "qcO&#Bo%Y%H4RN3u_dY#.(]l#I%PvGecBu&_K5DWkq<)s-.8-#%2G>#Eke)$QJW)#Fv4X7<f$b+D_`FZrXB^+')pb*/dZg1i>xOYTCul#E(;^]4p`#MwUH>#J#rv#o<dtL3=bV7C8G>#"
        "3<4#?&se>%@F3D<enSOV6KfX-FYs`uVe0@KZ]rX-;lcERvC,p7<3S>-GKEH4v:@O=`,b.hS'3v#:ikgKL*P:v&HM;67q-Z,$/;/U7(j_)Pl[on'EwfC-%MMUg)E)F=Xh--tOl--[U[O^"
        "Req#vG%E`#t#j4#Womb8B]O&#5e%Y-0T;R*&c)20PKmC-RkDv#v6i9Kx(doL?Y-X$CUND<;I1`$?g_l0]+Y0&<hf[#SugV-@BfMuoV_n#b1QUR$E`+ruFPS.3%`vufo,T1?doW7RP(v#"
        "VN31#8.v't7L@Fmo@b&+02LC%oULXMG7h]X)J>w$<_Axt@p)D#(,NM-hPP8.$ULS7%O.:)U/qo'0k@a57VlY#]pJI$&/>>#e#MZ#)>uu#;n$B458ba$'5>##-ml##B30U%Y^5n&YPWa*"
        "%jS,MM@fX-Fl*F3P0?L%ma:7uEnlMu)l$fUHT4G`_[-x$U]w[#D,pi#D?5PooohFVheZT&xk,K36'(`J=ZwBJv%ffL*1n6N^^@Pb3k-AOsUZ.P+e#ZGVNwkt)3]uP^W3$#r0O[7.DrZ#"
        "^ItcM5YCv#ld.8;Zk;v#.Wx>#&q9UA@l1v#.v8u&;:jl&Z-q#]mxMk',SBP8E/+Q'`Ha2MfKqq@-`tA#SQ=L#M$nO(g1^iB$-YFV#%%VSJ`siB$=uiBTGl]XrV;Do/qS2%.RK%M,Bpf1"
        "9sLA48RO8&hwfi'4-xA6>.iv#nTDv$h.vU@_rF&#wDBm'nYkJ-ockJ-(9=f-FNZ:M,N1,%-*BP8@xTYZ8``U._+rY7KBUh$bX<T%[HcY#Bb'k$P;,##3Kcu>*.C'#G%wp%3x68%FEKr'"
        "B#%m00`rU%gtNT.X64Au[+Qt$o%#9Scr7Do'7Yt7mj9^#Zd43)gH3$#$C1x$.`UV$ZWCv#RjIT'iF)+(K1IQ86Q8UDm:rp.g*K1pd(:3)crTW7H),##g;p;fX[d>#Adv`*r.VW-?oA'f"
        "axpkL)OSM)3uZj#n-oGR$gdeLd6YY#'i>DEZOwoI-l68%jln51(Dc>#'2>>#:K1v#*6;,#4AWPA.JuY#9W$]>sXd;%F(DM3:;2]-qH(8e$Z)8e2x6,RwN3,RT$r-?x8jXa'N>Gi$Y>Gi"
        ")Te&#B/+:_0:MhLxGsM-Ew2j6ga;<#Ld6O$Z$M$#Zr2B#&2G>#*^G>#uJlY#x3>0t:O1F*pM:l0VsKB#==F]-qo29/=QPiTCZCN0$sRlfMVD^u.K`Z<kkJWRBr%C#^####8Sk>$KU]F-"
        "LRQ=00>5##1GiZ#g25eH5B4#?2_6(#3^]>#wNED<qlI1(tYLl02M+_.<V5g)#,Go$Ugn*N+=SK#M<#buIhrG<>?fBJB%?UDEA@4oDk0^<qTQ-Hg6=&#L4PG-B1PG-5PF$/$B4?#n(e?K"
        "fW4=-ch'S-E7/rLcLJ'.PX>lLO`M&.A>i*Min[m-4eu^fRg2>5WEp<$'2G>#7Y%[#;Ot0(r9g*#3IC'#8>x-#)>>##OTh[-j+GjVgj8%#(o0N(g8hh$+e>V/=;k_$cC4D#dx+G4QTg[#"
        "54/IuApsN1?/mm<(Y[6KJ7/C#bmlcHtH1c+9A]f$t7Oc34OM>&S.CP8B&KiL;P[>#IUx[##6Fv#t^C_$/]u>#/]Cv#F#DZ#'5YY#57,H20jIH#&/5##s####olNM(tu(&0Zw39%,jic)"
        "&kUtL;a&e%Q0fX-`=sxb7/nRemmjC4>suj#u=8IuA4IU1mt4?Kg71M^mienD_5^iuRB.[T#CU]u.-TgLYDZY#x[%6/3f1$#Wt@lL6;af::]lY#LCs<'#bSs$4U/Q&C_*9%-`UV$eDe&."
        "-,[]4%kXc/@jURuPRj'KKl*LrNtkReX*+*$^&XfL2^iku0@T(/Si^Y#0>b>ng?R##Np+X7XYc>#(2G>#5;2Z#*,QT%o@K58b#]A,'HBW-O)c=Ao)Ph'ZlE]4bUG;-+rcK#lk&u.$aj;-"
        "LSD^uO/KS%6$wCsNbIQ8B;FkO7TR#5U2PX7d(DZ#r-@G;DG5##5l*t$wtCJ:0@Vt-SsboLNC?>#^sRfLekmR(f6+=$-wZ0)D0+ZPvp_M#X)vFumc_5K$f*A4Hsid+3qbkolCx;S6k1i%"
        "lFt%u](Pw$HFI@@]T]<$#.J'J7goQ8KlK#$cu,-.QlRfL^sJfLTo/-#>$(n(s0ro.bIt%uk7QX$QlE]45?x@k$oO.Cm,SK#*kxP/vJn9Ek#wCsXO2Q8UYY2VxD9%#a]:Y7?j;o&+GcY#"
        "KjVf)6xm-+x/_h$m<.lB/E_N(sZb=$s:R$$%Xf5/#XhG3;dKs7s[uS/W_.u-lXE7M>.QZKD]dat;>ED#KCn@tDgda1Ft*h#w%A,84b]2`B0)m$sBh&#J%7v7x27kXi2->>FktJs99e(+"
        "Du(Z#i.cM(.+VJ:5PlY#@J>d$^(DZ#)>>##ANs;66J''%0;>##Xct%,Of###HZOY-:Iv_+.+qF**;ZX$sPa&+F3lRaNfUs-G)T88=Ef`tUhH.MCNJRnv77kb'L&4k-&49u0Rj'Kq.E,)"
        "-iW7eDwiv8861^#M-qP'F####rDGP&h3GJ(>jTv7WJaHZQK<=&5MY>#:uRw#jTEdX:,.n&Lm_K([vY3'Ew/Q&_/*E*+VZP/`$QAOEE48&Xq,ftQS&CKPhNJUM%oReEtVZ8v(u1q+q2$v"
        "fd;<#xfcJ$CT1_$%;^4&:Uh>$Na`=-+X8('&FS&ZBd2_$qWh-N'8/3Q(1kB--dkB-G)&Y-rgr-QKRnf_OL.W-/%&'-C^)_$CZVX-t9CRNI@7/_3Xn9MNnjc`#=DJ`x3)/`Kg3)#8Nh3v"
        "FAc^$9GY##@W]W7Wxh;$/f$s$YaFnqmax8%Eqs5&]&*E*xX*Y$-1eX-%@pN*H6E/:5_rl/uRsM0LA/A++e1/vb/b$MgOEmLE_)_$Hdn`%cqJ]=mJ8At#8pr-6`3>5,,l/*`Cci(Q'bT%"
        "8cuY#8G(v#.#1@#jQ0[[lkj]7BJcY#5;XE$C;ou,wP:v#E?_<%^r.[[_),v-f?fnL_:nQS[DOJMbL3RS$GeA-vS]fM1rjRSE)&sS+IVPMWIq2)l=-##7@c/2K@cI)inur-rA(l0W*FY$"
        "7'?A4X5Du$NW^:%/^3u/OYj=I:n7I*uiWI)G.^C4>U^:/gch$'_1K.3?=nY#q9=SUFDi]/%?g+.mgYNJR0p2Rd;hCRh,dK=JR.QD7ux`GS*B;Do)13F3M?,Mxt7>JePO+Vb0ot0HpgFl"
        "SRL2Eqn>oMv?T#Snsm1>3[S42JECv7dxX8C&WL&Q&T7l=m####xl+uu$@_j$*Qj)#T8_e7SU[S%mOX_$cHSj0Uk%/13xh;$F4-Z2=lLj21uL?#>=eS%D;N5&,]Lv#;/>;$:,A-#HVmf;"
        "6(;?#13G>#;9XV$:)A-#Sv,v$PBNm1ivRZ$44Is$^.hY%Kx__?[[fv7^N#U9C`t/<G6ik1_#G#-rSVk'v(FS7xlP9%pNJs$VPi=.?i+B4ae_F*<]WF3Yu.&4MK5<.cTqc)xb#c*nv-E3"
        "9gDb3*/'J3'6@L(HcP>##2K+*FRnPfs2u6/f@OH9HCx8%q;H[Ap</vV4;j'-UC_/)<agkZ1p-.=<bKnKk0d8T9.;%tOfbFn]hcS%6Iw$?Ik6s:JIfH<sIF8&o+KH#-mZM*>7Hw].sK5."
        "Pp2PA&(1PM=H1X#Ca.nuTA`sud1o@,-kiXm(Kqf(7-t-tLRIiB2mtccw(Q>5-BmdQW/$A#NeG##Zlv(#.caP$Ie[%#D$h]7PLo[#7RNT%X<XG2`(Is$oeM;$%O,+#/A,##@7LcD.cUV$"
        "I_$dD4x:?#C]Wm/53G>#3Z]Y#;?x(<Yx,##uCE9/Y='G3@uJL(G:r;$VV*&+vA(l0x6Qp'x76J*vvNSR5wji0vZ2xKXJN5*Dae:T*a*mCu(Dg,=sUxSk*TcVe8HTBTMF]HScfY8V``%6"
        "BCj19:7A5B)?WG<KV#<I_b@PLV4n0#n]'S.qGk%kZ.e5/=(V$#__&h*.jG<-ZjVS_WN0W$aTIg)RbTfL.CRm/tb?uG8dMD%5bPs-`D*vLs-5cM]pvuQUkUv$OJ8SN_K&%#0TZ^7K7^2'"
        "8ol>#+55##=uk+DXKTq%D=[s$dV$2NcQ]s$A/MZ#N5<a*=wo-N5:xiLxd/,M/a[Q/:tQ.:,(mI:h':m9g1.`&h70vQk:G0.TKx1B`VlS/u?0R&i/_W-j7R=HswP,*UFva(&VEqgdow`*"
        "+#i<-[$r%%&Cmb*2MR=-E@7a$)Nwa*%Y4I,^tPfCjkUqoL/[`*)J/;6/Ep],9Id(+:4$/Zfx,##M#&,*?8uxC=WpfDTN=Acq'vdViM>S&Z)P:v(S*Vdb,G2Tg/fDEM]'##EwdDE>%mL1"
        "lJV=Y<^BT%@1wiTOs6R*D&viThP,F%0VWcs0r9s$3rS`t6cPhL>c;bV+f-s$S(*@'at7uGgJq;-fJ)Y$aDP>#,G>##AM1v#w5XV$]T31#X@k(Ng7hA$:aC;$>Q6+#7P/2BGVF;-@e9L("
        "@_D[,xix>,JP1P/PQPW.hv'Q9oB'_,Tg'u$pn0N(8=:jEcfxF4p4E_&.pU_#ND#G4Z$nO(Kf_;$ZB9`3QZrNp-D(b`H_d4W@&:REjNWe]6BDv=a&(bKB(8j3ow6Idvp3KIa*?ef#h8A="
        ",:Alu%$wi'@j$X:H31F6[Pv#-=+X$TF:u_jIoI8@&7DK<f0C>8m=Op/A;b=B%(5?#(]lN%YZiFeL<v/1wlo=#^]ocD](4;6HP2T9;AY>#HdE.3V4Z##T$aV$sK,+#X:XcM0rh;$nMS;%"
        "fUnf;*&###9vXV$FdF)<&]En0U$n(+f>Mk'GR9;6sA(l0A`i1l3o#c*Zp'<-Z4lp7)b;1)DU`S.O7xC#E$X=$fTFVH;Ojl%-I$mNr#qZ]>g;nVon/bY%9ce7cXjt^OtV&UxN)-Q3x+Yu"
        "R)l.q)9*KnaEgvXhbEdQff]d'`$i%8h31F6()`s7+IUr&KN[_fTN;MT&.ZxMtD>[..,>>#_e)hbHN:*#5*$[7LCA),wG-V%vn5q7RPZ;%/ndGtmw-##O29f3$s_v-f>Mk'W^il8tA(l0"
        "qVCi'F$fG3q52L1Kf_;$2ZZ]4#]@_#USF8sU(V?udRNU-*_Qq&xv)Zjgujo7P9T41)assuM/QruO?:28S2I/V*;#.8P.)B6EZ*k%d<h1cl9l:HAs[vP$$P*#g*K1psqm;-(%u$%(mr''"
        "Q<]Y#0d.-#wN<D<_:G'4XRG)4P%4DWDvPr#Jp<)s2qlaRHKFR<mc/M%%@'Ms5e]G;eLqltUx,tLvX*+.[k-YM_i4:6-itGM^:UmA_5rU67Rs@bwgZ#$]x]E[TxGg36lUv#Y=OcM+SUV$"
        "]9[g=`-WV$*3`C$21NP&B=.<$'v'2&r5*E*0%w9.0^k-$9V5g)[4@N#HftI($?O(<uY:bViwv-$YMsQa:2CkF]u[kFQCVf:bKtv#4`_V$?lt>$%<>-OZTJ[9vxInN7WT2u1b)rPSR#dR"
        "e6YY#N5r2;7V,k<%qX'&8(UN$r2<+M/)/48sMXm'<^9Z7@mb#%Y:ZV$31@W$@D:8%8sLv>7C[s$:2n8%+Nb;$6MwW?/38^HDbhp7SVc)4VRG)4^SCp.0sO(<ar#.$fuUT.1d6+r5S[I-"
        "Ow]<(p5[`*^Tfp7]qOu%[:$?#KN5N'71.<$EJG?#EhNT%,De20OkRT(Jb3p%U<J3M<0$I%*(X4:rRbA#^p3R0)0`BSbgs<sbq-o#D'I#Pq69W&4vWo7aed581&Ng;eTtv#&V/K%.Jb(N"
        "2uC?#jqQ<_`1n8%2B;,#@f&QA<1[s$-.F3N5+m=&p')UVJ*NYm>mGU.wjc*R@ejV&n2Ys-4BZtLnSc[0,]L;$:m]Y#lCpl8u=h2<10x9.GX<Vph4b$%79p5/h&;Do7It%9-o7)?BXsuP"
        "<Lf?K.SS&$SXLA&B&f_-r8nwB]QXa?WQ)<%2Mc##2*.;/$ndU-]MN`<Ov0G+&TYc2$QSm(?^?7&4VuY#0G(v#vhMa<p-&(,o$x+2Y^+',e@;$#SA;c<it/f+8o*Ab#nTZ-4?*Y&37.##"
        "sks5$[;2,#dP(Y7$Md>#:$cJ(+/,##f?ow#2O^20(AcY#1$###%Y0L,,)w(#+(901%`cG*'_Fp%$EQJ(T`CA&wr1T.pn0N(dJ>)%?@o4ok6r(>9dp1Ma`Y-6]$P+53&X8ajh4ALRLcIh"
        "OmY'Gkd5,gdiDH3Y($u%&)ul/:qt[lCWXv(TkZY#Q%q7RZcZ]bpg&/1BY)B-.DlY#kIJl'5SuY#NoP>#@n=C-5J>##X;oL(,uCJ:8lfc<$X'%#*D(v#gc7,MTfipL;/Ua#;/i,+eeSk*"
        "V($9'`%E9/jX]21-RFp%;=CF*JjE.3bJv7CrC+E*%Oi8.E$4Q/b$f8.S@Ds-;h9T%DU^:/83C7Kg$M?]3jIp/7,7sEY[T.Q^jA@]]G:?-jsFOK's]ZKCQ[]baqPp8%Ve;3a<>UK$Z/%]"
        "NACb.OS(t:%x]q]W6FRNc047NOs2t_/#chu/FAx$=Mb&#(E-(#LKD41jY_v-0oK+*G=@W$84r?#va(($wX'^#r/5T.fU&E#Y6]J0(V,K)^9+=$qC+WS3qUN(m]WI)?7J7&g8lL:^HtM("
        ">/%M.Cc&C4x(Ys?bQ3s_([s%SPv$f>e-chP10gB7=-XVa:/@N=%kiONE]$dV_)M9@GLb7)8:D/DIeHgX/$U@kgY#?GkKwD><3w^5K<g[Rn:JJ1*W.nAN3kT13B]30A:<sL=aH##>@-,$"
        "ikhpL3Mdv$Y7PV-FDm89,25##F%Is$Al$W$9rM:%s<g*#.GG>#<1:GDRp*[$@Y4.#1i$W$QeuB8TeKfLmrx],$#JL(B.i;$Q)F%$]A<E*@0CD3nuL#$L#fF4qKqnJW$MT/qF9+*9kR9i"
        "6'Xv-=?6)*mAr?u3p7=^_/RQNV8s+?:eqh;>?d<8W8wu>$9@CsjTU+tDF9iW+Whe@NlK_uDZxX[xRO8MgH'##l'p&vPhf]$me0'#v%_)$)J:;$>THD*G,3a*5+r;$(^cY#5]U;$C@m$&"
        "^L=gLv)&W$2.]'/eDY?#YQ1j;AWjS;Ipv>%Bvgo.X/1Z-G3/&,/D%=H@gVW%QmKB#wqpU@=a)<%,i5g)4aG,*%q4b4h@h8.<Q><.GL@lLW+qH4V]OFD4:Yf4VYlHZIb>W,RMOl]qDVl*"
        "A&qDQrr8)A%00j_-n28[f%3C#1B'W1e9+?-]Yer_FekJ7Afnl0B-o6IB&[su0MMnK1AaPAO<Z]+xk,K3.r-8`ZK+9%B0Y207/>>#p####R*R?g[24*$b8D>5[Z#=(1a<A+s-=p%C[uM("
        "iof+45WKN(N[;H*n8k-$I'2$%5ir=.=;:82ae8.WMIX03E:LM9]*-xK64#3M=rPV#ZISR;T[f7;uvJf1rB*dS,*WluZ)c54lhV'Auue+Mn)8qLO(K%#n+rY7ljLV?K>s[$7XaT%2`c##"
        "E`65&:j.-#HkW<U8gr>5pn*?1BOHc*Q8@D*&bWq.VW_A4,#rl0M,B+4k;oO(:;/d)g@h8.TmwiLI<Z)4O=&;%Oxq;.j'?aSX[_X#V9+-Uhe7dui.rxUn7r+>R/H_pctfh#Sx5[WM9SgH"
        ">qHVQraw5EUHnQWGc8]XPXM20v1ET.t$=[77owo%qE,+#2lhV$KoUS%fd<1#/]Lv#D+q'%.](?#,N=r$Xuu##ZD3#.nlRgLv21E<Q_kb+d1SL($F[t-s.&T&10[0)'BJaAf<g;.U1,hY"
        "?-`F*?mZv$T%:8.QXK7Q`G5.q24]OTQ^Dx#r7dhI=nbh$A;M]O;.2k#9?jfPrf@oLrXFeHAw$Bu9F,LMXKBvLxEMY$g@O&#DB@B$+>P>#Bih;$$<+?#&Da5_vqJfLMpq^%W+Cs6:SjPA"
        "gS`>5xj.##<[2D+nc@L(Q9B6&R`OV-;h$68b&?g)2K4L#=f)u$t0;W-#v.&4kDsI3*D#G4TIut^)f'%H<RTfUUu`-<1wR$ugDM6(s;xcYLXr[@7d(lq+F[[=J,ZoN.LT'7EChJ#?`K(B"
        "tE3D<6Wq6BKnBD*2w[&6Bxmo%bZ[dlw%ek*2t#01CI*p%]G7+#iUAA$QIQP.fcjI)O`vp%(Alv,.OBD33L5Z-wwaQh)oeC#4Vk,2YQ^1KM9dV#*pn1]$nl`*I[uruZwbc;)$i_$K;L%I"
        "'4Q:mcG%,ZH58Pu&Zq/#dDds-L#BQ$QV24#&&iY7-c`V$xhs8&7ihV$*>cY#w,=;$3Hv9MMd/,N0lLv#5Q:v#<t[Y#2p@-#FEbA#seD[,tZ'u$s.rB#3E'N(GtD7UVZe'(F)f*bXceuP"
        "fi[t90)TOM?kCa'jJ13=l?LIX;F9k$Q5>##;H42$,Jx-#FO?k%bdbxF7ART%;9_c)>P1v#nkiV$/%J_#+U[G;.&###+Duu#8*,##,GcY#GpC;$4+r;$C8YV$7CRW$J]Y?#Ehap%ao7T%"
        "lu/QA<eNT%F)ZF+p?t(3I&9f3'SjT/Hh))+]<wk(Sw3t$@hn@#e5[)*Je#%H4n7I*#0Tv-`GUv-@Tsv67BMs);%eX-X*X46u*G&-s*9xG)6[t9Z'3V0p-Y59+agp*fTV,uS10C.-k_S0"
        "dknI4MTM80g*X+5=ret(,wuV/EL:u($^1,R%/5##:@uu#[f#3#h,_'#m+5$872)X.QfCZ#8Y-A#i>&q7$if=%B@d_%5x:?#;=[s$Ev_V$2a:v>.UmZ$=8P?#*kWD<[?>j0ixms.Pa=#-"
        "@nV`+^T>W.-qH)4A;T;-6H9W$gv8s7o'62VQHSCs=*c`usa?a/^1xo%3s$E#U)qF$,-w<.g@3f1f'O+55vk=Qq,7,R=tNe$q3n0#m<fuGTEtoIInR`<6C@v$:rqs6G=UfujQ%mJC@Viu"
        "*4+&#J_-0#R9j@$g5i$#ROWQ93G;s%1R*W-nDD3;MFH>##;rBJe2wY>sM4v.K:e8%<1h/)Drds.8>p+*+j%,;l52O#;2mL^BWE`uAl@JClinc#Vv&iuag#)]F^YquXOX4kW>4jQ(Jg_#"
        "Ja%,D^^85SWisx+HP2T9O$os-d#Ep7dLHZ$A7HC%C7(gLdNj;$6j$W$S5](+YbU%,8[2D+1i6<-0qir*<6ur.$=CCs%%?WJ3d=@&7>2,R_?%;Q`X:s-kGfi94E&##rlm)#LNc5/BkT5A"
        "ba(kL&cn%+PA*9.Q,C8ISnu>[Z$qk.30$^7YukR-e31o-?0*.bZ0E?#=Cn8%SO4a<,^eA,'wV2/HT=>-4F1]p0bZ^p9e.JM?[H#u:XAs(jhp>?]JF&#mAmE+Dj*5];gai9t/u)6gjbP&"
        "rWp%Xrx5s$3n,wpqc08.&4H>Pp<c`*VqAT.voID*KCm4.i6[Y#&1l.Ck[1vPEQ(,):Qt>7x6.g:&(`J:8jus-lGjtCg7L3(C_+/(lieh(i=*#..bLhLf)#t$&EQJ(.XFx#<Q_#$<u0N("
        "0vr20p2a6cbg=Ie@I<HSQv`J$0?s`#Vbb48c3-H+].x:6$l2:mE.?>#2x9kO?CHkOFxGg3b9.w$.`UV$B+toR5J6p-6eB[KMt9[Kfa([KG0fX-L(,B#K7UhLG>5GMxLe?K@k#aJO/5##"
        "m/FE$Is60#$#<jM&rqX%xk^^#tSgMuCvfcM#9aqQCJ8%tiQJ9r2NJo[Hf)d2hN&/12D9'f=(Jp.,V(Z#:s1`ZO]_;.6a^;.wc_l0,D#G49[mP.XCn8%@c*^,@/7'4pYjtIiZMG;Hs&UK"
        "YKUSIe(*[>'Jgw'pjW/J6H6Ee*R[aGP^naGeDiG?O*C'#;n3x#5Xn8#*EJ*.M&PwPq(%e-TWL[KPHb5Vkmm_u_0N7)%ALV$mbM3MnV0NC>H;>GYjH5M+Kb'8J[aSUQe./1U4FGMG@vu#"
        "-W<T'$),##GobU%6*v0(oJ:l0xm@X-U%<;?@@c>,X)+##$2YY#Q`*x#>Ia-$#Am3#;Tj>)p7nP&:F&pIDtU^#TFEp[Z.t7Q4/&F-v<5?I.hAZ7jDG>#>dv`*DNfc*j/ns.MbGCsd)0.$"
        "0:;u&F)>>#qak?/?(A-#qhrR8iN<)#p7v3#w15B$QNc##NP(Y7h7Is$,Guu#ro2AFXZKFa7BoZ75b1N(wjSYG$Mi(3h5pM99@o`t(6+&#,qAo%J&gi9f-%##KIkW/Kx(uTd$@>#lf6V?"
        "B#/mfaE]f1B43?#.](?#gqeK>&_W$#)5%?$nxqB#rkh8.%hP<%Cs*P(.ob;MAC[G4_7D<750vYsFJ)P+7Z;l'n9.NBtD3`sQD.d)duf9D:/Om1Mc2Q/tp[Y#Zm8.$;SxA#Zwi*%RA&2B"
        "iT;C#,>jl&<`Y##FOn<$or#cEHt*wpu7gF4RlIhE$mO@?+B+/W1I9r*:p.i(M#HkSfFZJQ&;cY#xAf9$a`d;#<?8W7JDcY#-9ZtL=mVE-1.,dM3)MT.HNLK(Vo<//Np+u-b>D0cLR<PF"
        "n-9`%9jRt@Sl)fN>Uk$MJ%MI9]Hq'#R2PX7V>G>#wjCC$/kn`?H:,p:Tl9^#RVV*@'kCG)fs0Y$o%rB#Unn8%=0%.MM*39/9/TF4JYC_%h;EnDcvJNL/.T$0FdOXlDe..)jn7@tskZWA"
        "j,0:A[UqhL]rbP&pB')Wfk75&UH^#.P=#B#QtraeKTUV?)._^#/H&##lib3FMI[/)NG72.:XcGM-E#G4r]E+Q@SU0%%9ZZ@f]%^X4B1lL19Q:vx5vK1I)3A+l0tSnLxKS.&]nHHvE31#"
        "^w=s%02xA#/sd05P`UV$K_iQ2($nO(g'bP&Uk0An_*S$bY5a..c(RmM>^_@-NMe*(sx)V_bmbP&j:JxX4u:j^b2J_MZCO]FDr+p71+R8%Fgn>n]ig._goqpgoNi+$?&p&$^&bS84g'B#"
        "/Q#pM/D3)#:QUR#iQf9$E6>##rhwZ7-PrZ#YC>c*0`1Z#xDG>#&8P>#8vxu#3g_V?-25##3ihV$=Q:v#(a9hGWE7`$=?$)*djp6&PZTY$._:90Ehes$&jic)q<Ib>=rh%kS]XE;Jj$$$"
        "gHYiuK;>[7`TY;/n]REnlY0L39a:R3XP?q.J^brH<Ku99-$NB%O>,?5qJtE$^=92#MXI%#(bB]7(TuY#:x[3',ds9'BCR8%cMcb%4.@s$V-,##4M1v#AF2@&/i_;$[qi*%fJaPA0YCv#"
        "I?QI)X[1D++igb*v@#L1*jhl0jtT/)0t.i)]SSBH6]`=.jQ/@#[M2^#N0p[Q_*+tuNnjr?>5X:D@1O`]Y%l,gWQ4U$De4u#Vl>:v0JAMu&kb4o[kf/14$%<#$),##HCX*$5U6U4FTZ^7"
        "4F/a+Gg$H)NOnS%>OAF,`'E&F6b?(#64*T%BV4.#6x-<$O[)%&etmW_Q]Cv#ees&FI45F%gWia3Tlj31caE-*=X1N(jcIL(+v$m0J?P[$XtZB]('*a49k&9.ae_F*5?TE4a(tO'8Ah;."
        "m9[=%9OF]4]ZZUg$HqUdYPGB66$;=8[kA1;sV85DP,QJ*UF,3DcAOs6R[nIU3k(#RKM1A#b4U5RbevA6VK$*=Ud#A#4R7o[a[cjLqNH=n1I5n%7:Xc2uX0p7=j@>,?6Wf:#&wA#-AP>#"
        "%@IG;?]P>#O$G]#wjR>#1`(v#=11,D,]UV$>FC,DJ`E303Pc##m-/<.vmw0#<4C3(aQie06tQl1p:gF4_0cO:-wsY-;Ia5/Vw&>.8'ihL.jq@.lE&^FuI?GE$sYAfSEOfP93>Yu5O_(j"
        "B?b]#Cs_F9?Ilw=9W.X]9_Hl=K?a3KDD-EeI7NM,20aKulW2ULeHLq#RwIF@Ur)d?a6YY#6ZZEeiAnEeg*<J:fa'@#%<wiLT`JG;G1iV$p,,##'hLL.AC:,DMc0w$*J(v#nx7gL]=?>#"
        "0i&K16S^^%%),##uL(N(ZI0beN>E*e'/>']kKf8.XF3]-^g1^,^9Jf$J`kZgic[:/^$D.3Mr)G`A^UX#x*T:d5orPui)6`Gh5:^RTOr*1-00#,&&e1pURD5>RO]?^`ci6MQKh9Ma>QN+"
        "7p7XKO42UL3'Y-Y.k(K?.^3C&=b=)#QDFL+f?+=$RDV8&7i_;$?]_Z-.MP##IK@128.%<$A1S[#&2SL2L08nLZ<E;$:J4.#*;G##>S4.#:47W$J5kj.0]Y##nH+U%[oS2B44Is$U[p%4"
        ")SEt.DUHG*[3X9%e:WD#.*[)*D-xe*92HhLwBre$rfQ#>232H*#0Tv-7+(T/0*bR%5mrI/6AUW7t9XG%DpdC-w7w2,KwEn/=i'X/8Jqs7VknQ/V$4&PNDl+*2^e79sPxX%Ycn#6XQ6E="
        "6LS[#u2<.hQ_&@'Caeu%^(O?6UB_d<1+W?u^$%p.&JY?6k^Xu7]n'W/gZvD>C.Vv#LlKS.$b4=$%IIG;.;5##I_+]#FP0ktpt$ktM8YY#==_cD,$<b$TY/6C`n-##EtV`+,]'f)kJVk'"
        "ZCHP/C8fs&$:_F*iSw`*#-6<-RE&(%RsBaYr3=]bVT>;R_@@)+;P5$p+@LK2Rx=cDCs-hu2HN$6+JPJMkA63D=u8M(bsJ@uYNdlL?d*f4LNjpKefGSj59`hjvqJfL^0@##gqUC$JZ;4#"
        "_9F&#0It[732D;$.kR<_QAln(JdNc'`Y(v#0;P>#DF2@&./,##T)(5M`IK?LPl1Z-bfvS/R&6O0228L(W'+t$U;qR/drw)GDD4jLp^HLM:=Mo%WAh8.7EY8/Pp:)3==F]-+w6IZfuwiu"
        "apAHS.qtm#GgM&lhecqL=*Ggu<YQC#(C/c5)Sgg>epB*S<vZV-#pH0u%r'8I6kD35l%7Ee69KP8bXiR[qVRdt'+.g2]%xfD,ge&=cKb&#5bB]7Lx`V$0f$s$-ast&%rUf:u3rJ:/+35&"
        "1P1v#?4Q_%;k&q%1iLv#>2YV$6g:v>31Is$8v6W$l0AW.UiVp/^>1Z-O)C;.('F6j9f+q&PNjQjZYh;-hrMY$35+^4jB7g)Kw60)XCn8%=A/)*UH8ue`x8XKE@fs-D-t'M-=t0S+s3P8"
        "mTuY#K'DE0cA#UK<Keo@*M(O'PHVsLcf,d5qnS?gPrf%8[nwdmwhSFMh]s09-8rtuuRD(.I.>>#Z#n.CC@6vP5$8G;%]BL(dOr8.9=3T%^.Yv$JoD'-1<p%#47?c**_'t8KT)j(_5,W-"
        "l;9q'Inb`uTk].Ac3wpu_jLi'Y.CpQCxh[u^Q-N90-h#?YAmV?gmDM<of6n&3`Cv#vQHG*>DP>#(*Cj0R+mY#r5,##'2,##4n9f(mpV]+ijp6&MHkA#ERR,;%JK/)M4:b'-o50)G^LT%"
        "9=L93VMApChRPv.crJWIi#6H?4=W/D[c_Y6c*MZ/2oe=J&[HG*?lh[uGNlQBXYF]HZ'g[ur3k1^ae>9:_`L5<%5YY#VPw>%B[nW_R:)H4EJuY#-YCv#%[]p.sdI>#h>rN;d<wu#0T(v>"
        "-cU;$4WCv#iwVZ-K3gn/83&88xvbA#]bEm/h_^F*p:Rv$?bBP'dW[C8M/[#mu_clAu_Hl=]9>k02g9F%q&`%RKrCC?4[wP8,KMH*(;NY5,h)hLSL5A#u9IG;Cr1v#lPg;-FJYr$bC?9K"
        "4qAg12/FE$%),##8g$[-C*[*NKAZ)4BLEdXh?^:/[tC.39XX>N)boT#<E4@(H%`_?-(R*fC'?Z-giC^8F7CB#Nu#B0Xkp-ZGGv5/MXI%#<veQ:[*89&2x68%CfaHX>[_p7T<vV%1I$gL"
        "coNZ%`H?D*%`gb*b2;<-tVHe%ll;T%/o50)6o0N(m3F]-KtPS7RP*M#;iuE7RRgx+N*P$^_2447AMAtSoSqBu6=+##R9w0#Ie)@$rf1$#1ef8+xwn/*+Auu#'25##8m]Y#:P''%jf@v5"
        "$J>V7[LGk+B1?T'uC*t-j'kK;@4]>.VF3]-1fiO#l)nVU$M8GM0>1YG%?hJ##8euPKWaB/5&q#$,s['Lv2Jw[spGVdT:Mc`Bt'RE`1EW-*uhv#O478#=EaP]ZB')3nE^:%pcD<%5Qaa4"
        "I^_kDCK/i#'Y]uED6vcEX-5s/d?Z;#UdaP$9oZM2Mqn%#@B[#%%,>>#;4UcD2O<j0.SuY#BgC;$5phV?9?j@$9vXV$9`(S(LCV?#`x@1(wf`o&#M.C&X7Fg1vkh8.*W8f3;%eX-N-xe*"
        "(@hp.ZI4J<0Nl-$Fi`Ou:7g4N<I=2K-'@M;-*Ii;&Bo.Q^aWI44HYF55==g$1+l`*&A/A-ExV(ZDt,l$Dujf%&F;ig=[i%k4j-,)._%E5fWI0&nvBSI#k-0)Zon-)v,k-$rio>,H-f;-"
        "@u?6%4XCF*>gfX-`[Tqu<dE^RVishu1*XI^pRXluPDS%thLF<4,.:pQ01A_uJ=+##*If5#C)]fL0)[##5RD##-rrB#btQP/16u%4.om4&AXCRu_]Mkuowv4/VoOQ#DQOS3_V;T%Ol?8%"
        "dlfo[^SAa#FuTo%Lmjgu8A?XJWrH5#P(b9#ZtPR$Y;6i0<tmeM5h`s%;_G)4UCI`aR>ra#%;W-$HNT>$0AAe]#xqL#5o($u(cl2#uX9^#21PwB1YH&CcF`V$)nhA#8rT$$%gO.L>4xDu"
        "uh]CM90chuI2TM#DNxUQ,)=^#BI9_ASieD3=wfi'.N1[[9lj]73v7_'fo_V$eL,x%8C[s$9)Rs$wUse+/NgF.>_u&#3i1Z#^<_8/4,4o&rx&58aB>QsCtHQ8_c#UVQ.AP#q$]@W$HE;-"
        "a.<qVXJB+i%@nS#iORM0JX8g+?.TV6u-UVZls=G2sMo>@,Yaj0lhiV$-r@_#+U[G;082t-v-.,;=H;s%[Ep,;]3+F+hLDM0_)G^,M&GF+%#m%&&3GSLLiw*3Vm,-;vBw-bFM]K<YDf8@"
        "1?h#$C-w<.^r(W-heVh3&FlCN[A/=/e(l(5u2R&4$,>>#DAr:dOmnV-&S+87.OAw8f$%qreH$)*=>P>#A/*-k8SD[G)UdZ$uT&t[tI:?@0PdF<t)CB#qgCL=d%opgItGn_wwlV-3Z2p9"
        "VugI;@6vEREY@2'T[Zi^M+ho.Xm;nW,)_>#)>cY#@YQ`E6S27q(%Ya*.uv9.87X%$>obI_2SVu>QlauLB`?>#.]'Z,/W8vPP@HP/k#H&,Y$e^%'8XcM%&###i<$t.A?Cg(UZn?99H%B#"
        "wHNh#T``O#Eq`IquEXQ#11tLu8E9qu?lNrL4sv$$Ze]1p=$j>#h6el/Mgs+;S[Xt.P4%W$xa(($7.35&9uU;$@FqcD1Ml>#Z8uoLR_m>#i?(C&46&V036GA#YN570-fEx-Q&l^,1p%l'"
        "@ic##h2MS&T2o]%<<Tv-ogIx%DjE.3;F9a#l+s0(J>#G4ko)C&mCn8%SwC.3&=$&O=BUA6i+RCR;-1JHw.0@QF+qFG.`u.Q[w&d+t+Oc#Q6p,2)VA&QFND[]rR4ZD1O9xSg^#1MVH9v9"
        "Tw=uu8.L?9kqP($1OuD3&,,##YN9D3JJD2K^Yw%+t#BJ1-m:l(R_ii0eR;v#+8,##1;PR(1XW`<4WbA#'%Es-Y%ffLX5voLOI?##/1Ab.wZHx.mlKS.ox?[-]@;$#]HTu$b.,Z$nab8."
        "x-@x6f8vI*9Ze)*mYVO'Pf/+*A)6<.Rcu>#W(*,4RG.$$m4:52)3Lo<L7<YSkagL20EreOf2]T%;qs.D_RAv-B^FH#_,m@CHPYt,d4dh0kSYP&D&-JSF^gK0ROp_sD/HfSl4TvHt2[>#"
        "Rw]Qu`48nG7A=Cu8R4?.-m+1#W^v?$FGY##96u&-)5=a*WSZd+xRWR#Q.qRe7K9O#XrI%uLfj)uT:@8##)>>#sPNB#R,n(#]cil*u&><-DX08%/Xn7RhetC,eZcc)`/;W/*>cY#Z.h>#"
        "'55##8IuD31s@-##b&&=Epq0(D=7W$N&ic)[Se>>c7Bv-p4x>%9[SVKQK+m=:fY+PQQI&fBgH&f?<Mk#fZ?l=0kp4MQ^./Mud_2#mWB>$T^BK-A^BK-]wBK-g24?1p[eD$9oV4#doUY7"
        "Jln#1;.)Z#9@.@#xBRG;mTvB#;4UcD*JlY#B_^`*reTj0N]aI)M[es$Zoa]+8x@H*Dc6<.m;*1?S8#d3BTrv>#;TJ+4hk0p28)9.bBWJQu$030tfYiT$,>>#*DnQW4vN2BXkDM0D_+9%"
        ".DG##6J@$TJH(v>-I?$$0WS>#(5>##Qi'L2Lg6-)RUIs$(jic)pp1K(P$#d2O+A5J5p?cD8sP2B[u6t)--FI$-,Vt1T>wCWF'=p%A`ml&p:k22EF7<$,8,##m]sU-Deh).PjYk*V%-hL"
        "1+<$#+ZwwBxrlA#bU5g)&(I>#dVY>uGw?o#]jl%XY5@fqB7T2Bn2<$Q.)LGRN`4v,P(EJ3_2MY5@q;&FLjXS%fS7T08oLv#U`FA7[r8)$(55##/6YY#LV9W%_^?ijSgtA#(u:8.^-I>#"
        "'=`7R&o<nu*Mmc)C9P>c'Oa5$*(IIqt#M$BSI-##/SI)$D6+&#`DlX7%iMZ#8Ie<$)AlY#DLOX$tT(C$*8G>#>X#=((ZhpL_[b)#)(pG2.t]V.tgFp%$EQJ(l/_:%r[I+%H:C+*bp1K("
        "JnbU%1_D.3OKgw#nPc-V-9G,2dqt_?Ykc#Y(UOh<=K.@$-G;1D=PeC+]RS[@l.l@?&hf#*IckO]-CY:B=pIfL)D?2vk_hr$US@%#fJuX75b][#Oj[K;0u_f:)Gc>#jGaq-7IX$0NEM,#"
        "G_*[TX_j%=7q.&,CZb:.&CJs$(s.)*_u3I)K1=Q/*^B.*2LIP/AJ>,MCh9-4e0EJ:1wS[Q@+4LGjTfEZZaDv1_C9Uu*xIQMr(d2OBxLe$T1HpujCcIU*FEech;0VF@FmT#hBF:m3EciY"
        "6hK[?]BDx$KU0f%6<vo*GqHI2muqB#kZJ[#TYpSn$E[T7SxZ=.l;mS@copT&+Kw8'.;5##'f7u-8po+MFhML)j/_:%-k,&4eHl%X4EEJ?dU^YT^BCpuvQoa@k`[@#;2e/11@aP&*%kS@"
        "qBol&lfFW%wqUf:qImM(3JW0GL%^h(-.Jw#lo9[HnKpiEr-[`#x+op$Hr7+aB//s9DeZi9MxSMjQCQ:v`P-on>SAm9gfES*D(]l#:ZU).>Rt,OdWO&#MZ9#v=]Yf$W6>##U3=&#9;WD#"
        "M+F?#)Duu#2oCZ#G#mr$cjIh,VkD;$EOD@&4%M?#:1@s$CWcY#9`p^%cSer6)8w8%9vXV$flG)<(4JX$S%WA5aol2(^8hGXZvVE-@LVhMf^A@YZvVE-D:v0MdmQO-_PVS%7tCg2T[-g)"
        "x;v]4J)ti2$WC?-iAYb+/.$g)_[I<$Zgln&orqB#dpKB#P'8a*)9Ta*e[YjTpo3JqCOhkDfiLpL9WnBA<(lT9=2LG=<gWJ<<BJ4RWg;)NC&Z^9mlw%Ri%P,@jLn(?=$+ZBeH`XBU1QGu"
        "9_r6*D<gA@K[n%Ri/]mD5H&7*RC$##OZ9#vIJ>f$Pg1$#[-4&#.jd(#fhc+#^uT.,FGoY,GJc>#FjO^6rJPt$I3lJ(HXnS%Lk?dD5DlY#MGO'%x/O?#FuJ2B5r-W$R6vq9ZQr41E`J5'"
        ")=n(<OiFr9)D)d4n1?036$i41n2c#-V6nK)iYr4'IVP##Laqg(g*K-<WWf#.I,etLk%oD*MeeUR97[(%U`?t.,m@d)4PI['gY8f3pRGKF*xjIGktt1=$3xF4]+^h(F>0[2+?cC#K+&dR"
        "/AJ)SgAo*.q0IxSP@eK<&-]+4[r8L(Luv?=*?+G4)PX[$(>KZK@;B?Kokv?8YiO.3m_'h;aG0fuV6tDR@v?JQ:*^SNwWl6Dj_.i3Ue;kLXKGH2&w6jD<_v%FC)fP8?WX8%GU:J@Rd0s."
        ">deP8+)0.-+4>U%JP<`WD9pM'&dPP&^O18.FE<J:`?XZ#1Guu#)>G##I..t$9r$W$`GUPMnj&W$<m_V$MjHx.0v`4'[U`i0pOjt.Q_dc*R&=T'n%3qgte9[%N&7u8@,'COP#qO05'hi;"
        "03Ri;%V+@N`<a.N95(5S4XZ?$)PZ(52P6G4wf>W->AeY(4r#8^SD]%t#C3s$;b)##V9Xc2i10p7Oqbc)=RcT:8Jo-)l$dT:q<wq$K0fX-O'.0)R^&g2'ROp.UYCf=,i=HcQjU&$#Md_#"
        "8PwiI:932>:665MhDs:?O'>uuk[<rLtq$tLlnM$%v6A?&<+7G;?C*4:#BQa<)wT%#%,>>#EEcY#]ZXw%Vp<V7gAf,*?lBW&.ATB48*x9.T/.Su0%Yi^C>mV#$4YY#iAjK9P+4m'P#UgF"
        "')t.h[cbi91k1S*Wl1?u$EO.%eY`=-$Vh-N8k8O9C`D8]/kYR*P*DS*ombSAwqiJ)t0'#ZgK+<-`C-f=bWE>&2P8&$4YtbD,Z4*ZaZ8%#GqN4&fN0f;5GU'#%[ND<YD_;.VZ[1C[v5g)"
        "oZ#W%[M,1CG#Wm/axh+>^$vEID%###VMQ$vs15B$ZN7%#m+rY7;h4]#x6@G;ik.s$[gem'S]C;$5a1v>1ev?$;v+?#-tjD<nu*.)Y*7W%g*DpN[k(a4wfCs-7wbVR'5uD#X0'I6,D,J:"
        "HvI`u_v>68s*uAue8GmCExJTB`i?m/]SjH$L[L$%b8v&';8+DN<o*/OdLXR-gMXR-,pXR-`/+(2)8P>#:Ax-#(G:;$*&CqU;VMG)t29Z->8;[eHi0KM&+C+*^W2.0DpZAfUrtm#Rgw^$"
        "Q.`5KXNBIq=J2RKBk*I?C7rCQD[UDe)hw3kT####o6p*#pg0F$IMc##p;Fd*@:Jq.-AP>#Ad/9;/(Y&+cr2]-8*x9.InC#,EU@/$(7#2ukUpP8T.STAWVvlJs0Fv>TXjm/gS5/v>Q60%"
        "%04/%cvp;8/%]p.77*T%_RCT1^9E?#+JuY#BTG>#ao5#?0U6C#5v+?#tE3D<47?c*D_EFjfl;w$D$xP/;EsI3p_rS%)H_Z-.HUD3$u:8.<#G:.)wH]O807Z_>?o<FOFM@#[Thc77?7@#"
        "*eMIPh[]V-M2;tOIK]M#h1@>53@6ZtD$OpuNKM`*wX15/c;^M#GmX59[;Ev6^DOCXeviW%3]uY#/^UV?+(L^#8po>#&E8)=&eW29N$eURRU^Y&;ZJ<*9WD;9;MO]utkGW--(s#[0x0]("
        "M>uu#[R[oI?1]I%maHcMYo5`HGC=+&rEl`#mapT9@e*t$CcZC%X_d>#Gcc?#%'ed2Jb-dDA0B6&?8l?#@kA6&r(@['$>#3B@'06&I,ZF+,8^l8rUM)5_F2L3Rsb?)BtV7UOrFd0Q<DFF"
        "n<mhkoDt*./qhS0en*f4?p>T.&>uu#%]S>%kFNP&s=FS7]W>9%>:IW$Y=bcMA4(,D7h+q&?ll##JdGn&<[aS%QMc##^V7L(P@RB&F*x9.;F*j0xJ8DEGGGiuICt:dt(QW-L'fO3[.15%"
        "VN)99U(Mjr+*aqQN@eGD>hX&#%JJ[8auIj10mKF$99>>##hr,#V@@4=.&gcN^3dA4mmhFrw9*,`,a9s$YAP`<2ho5#Wx$lMjO-##7?/A$eCJ5#P$ev5MXR@#3#o+M$'8f:$:onnClJ]+"
        "5/mFWUJ>PSVa*vHx`fM1>W;m9Ia)'H+<JK#@C/]$Q0@W$+jq@i$6/G4qB]T$wOP'$cc?(#Kd''#4CD>#4oJgM+gvu#So%Vdd-A2TKodo'BDDV?r]Y>#v3@G;pnUf:<.5d<(qK%#GHU7B"
        "(&###;3o>#6S82BD2s]+%J>V7In[U.'2>>#UUX7/6GKL(A#.s?c+]T&Db3X$6ewG;okE-Xl(tV&@kkiKfv(2M(;w+Hdk8DI;tqCH`3]cG#hbd7(%K^$?3NRN-DGWL3V`IJ<IaNOLPD0#"
        "4rC$#Lt)d$tf1$#U8JT/,CCW&,O+G/Eu1v#NQOkMGxnIM3TcGMxh;F&IO/d*(*0n8,6:O+[QKd+(pr0(iv8Z-USQS/^C5kr)RGgLaXIi.h$RP/gPl=*7s*Cf_vg.;h78F#[94n-B@nb%"
        "lBFKNFTG:C_eL91IP+.m<8w&'WN#,2]tBg<'Auu#@ChcDPkXJ%f4@8%2+I8%ED>?#'5YY#L.>W%V[j%=wX&q/Mf,##/e_02'xxm0d&0e*b/I=.>I9]$t]5r@B1r#?$/b.h5^Nd*LAYA5"
        "?>`ZKgEM0##)P:vAOVc;Auw]4+vs%t_Q2QURv@o[cjC0MAdx0#MZd$$&4Rs$(Y0D4bRnRnFUOVdnGLM'3&9Q(Mbns$'t/a<@``#5XpvD*Wl?8%B1NT/o)ct$XIQP/F7;>Gi'61.8;_7+"
        "AJ%`anpQCu:7UP/V1watV@]n1Z#>(j%'2&0sN>+#*ObE$<a3,5RxK#2p.?T''/>>#425##tmI>#u:>V7?XeQ/*#-9%stUf:2AhY?]x6g)>Kx>-^;Erp[s<V7rbOn*G`51#Llvc?.7*F3"
        "qPCl03+&s$WjMa4EL`v#w'hIa(X3o7;CmCN67941<d7s$Ak),$7%Dv#RA-'u0c5MTDaH)#QUHN11q5,M5PEx>-<@v$_uht'bwgiB$fH=l1;[],,uK=Y*gZ313LVS$^(dNuixYY5+LfUR"
        "Bi.C#VH^s$X^`MB/+5?%gZQp-4O-3<NC-##.3t5()ob&#VJwgLj)uT%#xLJ:Nr9dMw<qc*<Rip7&Dx;6(F4x#+tZ0)RmVa4URrV$gJg.*Rd$o#37Vv#]P-'uw=$5JI?QDu>A]-uDv/q1"
        "oeC#uib&X)9=7w#o$Jb/odwI_:caip8_Kb+<b9q%=47W$uw:J:_3/+%&?O$#OjZ)*Av%C4SJJD*^%3D#x4oL(&3n=6CF8QC>0[:QCA+?.mNZipDXit7/f3BG$Mvf17Wsq,9'^G;s<n(<"
        "5oa+M1RmY#(s3^#OlwBM,4v<-.@iT.7`1v#pH%AF%LJ]]6f](NG.(58Ob[?T42-J*W2q#$4n9u$;.dWGfYW)>:OSa%:o]Z%fg7)E)wAP8[8-$$+5uFDsMplL)cwj'kU`BSC=Is-V,n`*"
        "beWq.QO.s$U1?nElNA#G,v@I*.PcY#%HQ:mQrR$$8B^r*rEXs-HZ(l*t^jdMKPKL(VccY#C*E>0,sSU'3%Ee+X,i$'Og8:'7]uY#Er$785m/,Na;Gq73<RW&,M;B#YU`v#Db;H*&&MT/"
        "mvDWu(apW$>GfX%;)J)>SBPIUr_ZSBj1m$&b4TW&F%6:vpWZB#Kco+D-R*,`2F5d22/>>#L#Ac*Eoc>#1GiZ#B(=x>;>@dD^R.s$i_H>#u5xF4R(C+*U:)?#D[7s$xVY8/UGn]4.lJ`*"
        ":vLV77<GW.?wk/K$-Z.q<'N31Y63+169lJK6Zweh^aX;-uhCx5T(hCE&$=20dcRf1ZGVl0iF$)EbRnRn(XTVdX^jERe88E'Ef$r8:7vAQtr._'Vk@n8e1-E3i('ig0oEB,0J8CW[GHA#"
        "94$igpW2fu5Vj22MQft76HWqG#)P:vi5+AFt2,sHHa-gLfU.9.YTT`3b*OJMEQ*%$`Zt(3B(e>#,Jl>#/Yu>#`'r;?s>uu#TiB>#WN<1#-P1v#+Q:v#1T]Y#3JlR(]0x<$p(rB#>&Ws%"
        "7RG)4x*acUFW#r#uYotu)=,J$<eO:$&^cbupnv^R$),##$93P$FDp*#V2PX7X12X-(2BqDVoHL*`FDDdVtQ&/hwVZ-R#]wB+C#i;CDg;.`5LrK1_1N(fKN)YOfmmL;YQ)S*woLFH[QOu"
        "'`uYux3kY9iAbpT.Q1JGG[a=YJ#9-vwDDwKhnFwKljH,MF^=R*2(%##f9]MTN8aDNV:E<-6IP;&.)IqL`9(sL,'Pt$e.Cj$B,0[8Z>Sa$-3MY5X:>_8Z2US%dGr80_O$X%(;YY#$QcY#"
        "$`nQ/U6YY#'0;,#>qbUA';YY#PW7L(qPCl0KduJ(aawiLS,CE##HK@tBfl9&w9.v#<ShnuDBlwL?86L-6erA.Rk%-#3o8x^Z2o-#2A)S$mZ`=-vXtR/K6mLgE_M,;em2=.fS7T0RfCv#"
        "-Su>#3udh*vp+c3FNG40sVLl0=xG>#0*8f3`GUv-S:vd$+0fLCCYo/13USt>hU7fUsNBlaAs44HBMncH>,OD?#vncH;rwc#8K'`avi.v]5gonG_nBs.;?t.?,Ifk+4=bMC%=sb2fm.T$"
        "Z;[0#B@%%#]:AZ$KL8e%KVCv#9)>;$+Q1v>,Pc>#2NC;$@h9L(7*nO(wc_l0_q1d*W3.E3$OBD3Df*F3-U=;?5m;P1C`Fb4Lpp'&:+@KupwmM1mIf$$.xWpueL6O$l2PuuHDBY$wFZ]M"
        "/amX7jucY#J5BM;38P>#.#mY#n^@>#)DY>#:vxu#(H(v>i2faHgL'N(,5%?$xqJt%$QBv-oUFb3:w)a46n@X-=F(;Z$,oUZ#?/REUBr,@$25Yun$prdq^Y=Y>(9P1[GAb4H4G%b[vPTM"
        ".QQB$:dS_MC/<Y7q7;Z#x,x>#6J4B#:VXe#nR#?$Yqi*%UDaPA4B-u5xm.##hAMk'Me*9%8Fjl&<O[s$Bp()37b:hL;D=/MPG)W-YFH>#qO%f12kkOM6b5oLekqau*&CMK).Ln$bn2'u"
        "g?Y6W_a8c1jU.+`$g(5JNU8R/AflY#+g8:'c8aj:@YP>#,^G*RZcG*RN?O1:@<qN(.)i>$p(rB#_PEqMClh8.bRrv#Vn@X-oD^lSAW7P14loUZ_sVtC]mfXuHARou'*j^u`Ho_uuF`G#"
        "x.wguqbVS70MtB##:^fLM3clo@aZ`*sqH^4tQHD*JlaOfXZ3T#0P`fL9c0fuAAWMGmI_&$G'k%bZQT)33:35&mciu5J=[>#u9@G;0M1v#.SWfL2Xc(N0]U;$Y&coLcEW;$8aC;$$bG+#"
        "J^5n&IkNT%_CNY.T^BU%5h$]%-c5g)Djk-$*#[Q/pHK`W$^$)W3qD[$93^M#%m^^#`gbOO(A4H8N@5X76/,##TE31#YDZ8)bX>l$QC;68D=Z&Q]l6]-mHC?%[AbV7GDG>#?E#:;urRfL"
        "<8-##$<'<-fZ+-(Dv4q70W^9VtGYT.d$Up&qs5D-+j4c-9p%`=).Zs.e&<a*Y?S<-49@f&3?$U%D,>>#Uj*##6X0^4Y7PV-U]XI)bpGn&EL[s$6O82'J@Me$X,###)-4.)[>sA+Wl?8%"
        ":o0N('.Z;%5u&.$+Pi^#Yxt>B]IhLO5=jOFwh3b$BI$TG<.kx#wxj@4)>oL2PJ*5fujlj#SfQrLu^Q:vN/;`W.k%)j[XmSRtiw+j-4<<-?,(-%ugnuudHe<$27b9#eKb&##3h'#Ps2_7"
        "#hu]#;RI@#.('41+P:v#A1S[#DE)f)o0g*#W7OcM/Auu#cqVW$/S1v#Y5CPMWk2v#Ius5&m7Wt(3@-o1s1nW.wq[f14OG70vI<X.fiH<.22s4'?ic##mG%p&:cDt.rZv)4L/'J3Cf^F*"
        "Xq[P/wHtU%.I/[#9Cn8%oo29/<oxfL=E514-<xF4RS)eH7UQla+knJI>8^%,=UbOo3dgqu6-2)*_K&2hfMC,]&3Ghhi2P[GqbLW)tQ_8<=475H;q)2bItJ$C@^O#-tn=Gub[eH@u('#6"
        "GK7e@tTfwGFx]S)P*1(HKb3_)^4Nd4w'#9#B$(N$D^L$%lh51'2Y4R*L46lB/;JL*)YAo#J(nIKIdwu6e<0#$w4Be*Y:AH;o8L#v&eGR$_gRK:&#LR''2>>#D0h1(+%`f:W2w`*fxmt-"
        "3(YcM^D/o)joPD<$nRxF.^MU.M/^R+1]KwO$R%8R+)xr.o&r)-NJPq$<)fHui#cuP]aE`+B3KuGg2)k#GqFuP%dP>-ZHEYPYFi<-/A;J)=3:D3O$Kw#5;:6'((if:ia.lBr)%(##UND<"
        ">fk2(sVLl0I2pe*+U[w5tnt%XR:uR@r+9b48O%h#J(nIKsL6P17hm=u`](T(2&to7O_68'nMQ]4]#`q%-S1v#*#-9%.DG##.5D?#v'Q6++8t**%uS44QGG>#bdK#$.8QP/ri[D*o4`Z-"
        "+_(61wb8o#wuHM#Z`U.P*O+]UduuM93d5l/`Oi8.v..0<(q=m/;t'WHd6T:va@Kw%Nd`p0r^s$#'_-u%U%o+Mm2#,MGp2v#n,>>#+MNfL[t,E(gGUi:.J0N(KL2d*kdUN0kWL2Up5Z31"
        "pnt18D%[sAT/NBU8%*SIBi.C#;5:=Joc(SI[No<-aB-h-KbG&mim&%#h7.Z7/fVZ#cSMvUOi$W$m+A4X=9AX-,Js0(wc_l0U2&.MsAm<0L7e[-4+TfLIk0g2EcWcVs+N01u`Y0$lbF:$"
        "7MgM9NY^04xbxo#sST/#H*twTZs2>5stU+*;Y(Z#(g8:';iU;$?tT1(+xLJ:.:Mu&RSuY#wNED<(0?++n'LJ)X8W&+sdrI3k/5J*`6^v7O13T/.IT/L(Fs*Tsavt7#2=70s>@gB^#;e#"
        ":DETap3jI2:Spw%j;0.*eSDFBkLGM9l?$XLD1@G;pt_f:/;YR(HM-<.ZtnJ:)U,F#+WlL'euA9rK:=M$R$b9#WAYCFG%'x$xDQJ(`QRS[C=cIh$;6/1`e^&#4Fg-6RMSiB0^tY-S-HAu"
        "S+loLpFArLdks$:P+l$/uOI1KE5F:Df)KU3IqGft[$0sNd($jVXOGGM$x_`jJU[*7wm7?$x*hR%j4ukL[7=u.uoMneqmgR3d#KU3Np<)s]-@QM)a;;?]C'V3;2CW-j]ZR33%fZ-O+fJj"
        ":)l4SgNt/)T@[6S*5K/&gW/W-vxrT3(c?X-a)_^u(k0O.8X&Ab2hNDbvP6S3?9FPV)a;;?5BCS3]6QU3D7k+)]KM#$I#]=.$ZL1K^Ti>$4x[=.$(/7napS0YDtsDO4=bA+H`EU3)n+=$"
        "jUv*&J&###$J@;?Q]5/MD90[%n6EU3'LR>M?W:a%PNs&$)*Uh$OkhgL,`Q##HGdx#4E*b$@F?mAxTtM(KSs+MLX=%=)O<%='p('-*#Qt$#v08.3Pd>#Y=bcMEb1j?+b3j3o=X&+o^*@9"
        "O9V-M4uT=Y@r5X:9s^C<W</'6:]]:MvSqAQ'*O//VK31#9Ei_X+*$Z$i3XG%JuQF7c7q9NQ$T)8bx'Z-r=ClL$`>4MPB6##'82U%%`DM0:fd>#XQE&$Y1q>#[CbcMHakrH+/###9:[8%"
        "1Muu#=tat$P^+W-eG,:;6+vD=T_=+MH'Kd)V<)at*'XA%GJ.2.;vH6MP_#ZG)()TCKnw5)U]W]+fh18.vsai00);D3@4ku5P?DP8aJt+;qUM]=+b'8@;mViBKx0DE[-auGl8:PJ&Dj+M"
        "6OC]OFZs7RVfLiTgq&DWw&VuY120P]A=`+`8e)##<kY)4OfT;-A@;=-W:GDN^D4jLilk0M>[;=-d-;.Mq$m-Mm9tSS&l5.V=NgSN>s&;Qt'w<CWV&I#T=2/1-u<2C#fK^#1]a19)+h>$"
        "#^sfLB[cHMb>C[-4M?X1=a-nMe.1lL%k]gNsVaIh-VGX(=,+p<Chb2CA3#AS#;O]u^RY9..2WlAC[3t-I@P3BqcZd+IW5gbbSi7er_Big,krCj<vKulL+&Po]6U+rmA/]t'G1v#7LWP&"
        "GW1,)Wca]+hn:8.x#ki02/DD3B:tu5REMP8cP',;s[V]=-h08@=s`iBM(:DE^3juGn>CPJb4>WJHPp)(B8dG*=Mc58j?$Z$kFp2DA,>sQVNx>-83%J&+^7F%d@l:2_H%-MQaWR8?NqwK"
        "`fx92:Nw_&vigx90-m#:&/,?.F2dIR4m09Q-7L&8Z/h>$+1MM%Ah<>d^fqQ8qJt2CFq:8Cbj661ZCaf8/S&6C&J,W-Vtge6q+0&GQ1199@<oY-MIl--00O$M`g'h$IB5B?Akl-MA3[GC"
        "+]4AuG2@A-O+)iL<PafLVV_@-.X_lL,)^,MV0HU82Bo%u9xGtMSMGGMnQ6fR3nFkLeCgOMX(H`-$KY#[<W(BbWdOp8F$Fd;<&*)#LVs)#]1g*#mbY+#'=M,#7n@-#GH4.#W#(/#hSq/#"
        "x.e0#2`W1#B:K2#Rk>3#cE24#sv%5#-Qo5#=,c6#M]U7#]6E:._$F0$5+&V8)htM(T]h@-IAT;-GM#<-EuNB-[FlDN]a.FNUDKCO'%m-MEB:@-F0-D-d2G?-hEw^On:.e-U2/^m?RVT."
        "-pZ9C>@gV-sv&+%#VkA#lp/gLk;O(OhoH=.ehS9Cl#7+%n/aq2PnKe$LN@%'c'EaGtwTmA_^&k2$,>>#`-%##-?\?#5N5[`*H`*YfVO-##J?(,)@O7s$Vd>e(u6,(b#IN@$_3-mb_YK.S"
        "$:H2c^SB.SvK2a-aKcF@G?>G2@xd>#4)GI*w9J@#p':J)Y>0gMQ'5sQ)5lX?_8#-3N29f30YgrM*Y+FZ^unuD^@q6Dm-_G.As[YDR,:pTI%br740h#$<@hcD6A5q'v6J@#hhBJ)V0wq7"
        "PxkM(C<=r7.w4Audl3_$VbBW/r>#&#fOS?$nmbU%H>YY#;?Y>#+EM,#6J&2B(AY>#<$sx+s>ZR&?I@@#;rO/aZ<Ss$3<X/42[uh%EUe8.AmK8*4.Mp7`e$W]%*7>qD5'99%V&0%ZG=SN"
        "N`g/3)AP>#Tcr.LT+4GMDd@-2+25##JKZI)U=>c*/I2%,fN9V.gm?d)<t/L*,WNO8`8hJD;`=K*l16X7%t/C/Ur^Y#DBr'85k%E+?9ReMd&Sr%@Pto7l2UB#/VB+*J>#G4i&>X-1La,O"
        "MfPa(A6r%EK3oEZ0hc1%Qa[D$(9aa*HIh30HNLK(oD1l0Q<AjMR`wZ8K+$?$d/if8RltA#t[O?%lCba*nchT.S@.s$cQO@r6er6ObB%n&;w=G2nXCpTbD$X.xs.##R9I$,2mQv$Kv8s7"
        "$`.&GbosA#NW3K(T<6bZ9?uu#bL+-2b^''#Acd++;55##w/o>#<o)T.,V(Z#6'%a$TT=;$4p@-#VxRfLiex],2(qF*#Nm40W4:W-1-+^,8)ip9NR3)FUdUS.h:S2)C&'QDf]khN4,e'Y"
        "IFqI#.N/a>d<nEZCN_TEP-$Z-hQgv64QBE+_YKg-KX/I$ZIV<_NMcY#C[6a*vv,k0Mf,##DO?c*90fX-3Qa;%0#aL1EtUt6`T^:/.fP,*:5Fd=[S0sThPWxbc?\?4Dfj?%k/asbQDu:U<"
        "^6,qiREVH@N8&^u'XIU*qx8;-65(A&Ug)W.q`Qk-g7j<`EtUhL.)AX-vYXV-O3Zi(0Y+FZcNvh>eL-FE,^R[uJNJ-Rou1bZCKC9EZS&^uJ#r[*8I.I/J_Xx-*AK58,Td?$W.?;$,]L;$"
        "?wkgEZI-##Vvb>-t2ep0oB/[#c[VH3NS>c44C@X-@L3=-83TS%D2o77V/Md*,G^N-Cn[Q-hCOCMxfd6'[Qu(38EX#71f68%*AG##BD5?#/]Lv#ZY7?RRuHW.Mf,##'Ch,3vdS<ADe$68"
        "V9BU`,U^:/`E4*?@]d8/MqIpKTXSZ$qq[YD-TplL%AmSLxPo(feMP<-;.UW.,EKPL(N#<-qx8;-I<Up&e@gr6Zh'K<],-m&@aMAfPq78%Z%c;%Bn3d3;e22/]17x6h9-DoAdNGobf)T."
        "I5^+4w_tH,_u`Z6FBa)+.l(^Y_BSe.kfh@-Z#Q&'k-(,)8eZi9[*4?#0Y1Z#/$-tLP<jV$Vx#?#.PcY#>=_cDmrQW-fBC_&lEZI)vn:J:(8R;U?Q^O0Fnns$xDQJ(N_iJ:/,/jim>w)4"
        "s0(A$_9*p#cGp,KpdiZKmI&?g$7_O=&s'b=,&'iN$tTUd2G$oLUK]>-1,/>-.vAgLr87##,L9>$)-w(#DL7%#$C1x$(5>>#?=UcDE7%_S$6uoLa*I##6QlY#wN3D<>J###ic*e)LiQt1"
        "6L3:.[AW&+:nY;.s>6Q/99L)30eeX->M=FI_t1#,/s7AuT4bf-/h9RWTMO1qk=&qKj:0#,326I3JbAW$1Mc>#*DP>#0x0L,MTUV?rc%Bf/klgL>K+e)h`%1(V;Kp&]HL[KUf*F3lV5W-"
        "-[AjkrlG;H$]@x7`=Ktqcn[@u$wX<%v/C4=(LBoLD=?>#4?T/)EMRG,q^#R&+=iJ:%4)7seH:a#P+vP8rZdJVi/%J8Xj,KDM<4bZ15_`$QF1gLMp&%#[1&?$+AcY#nSCIm7'On%t^(,)"
        ".gHZ$o%rB#h1]J%^co>$CajJ%>nd,bh:0#,#2Z<-p9PE:mE5s.kaMv/.FE:.OS3gD&6j6aZ'l<_KN:;?_UIp.6qVV7wCU?^&d'##+NCV?*Ce20105##H?HI)jrfE[B,m]#5ve8/hGbp'"
        "(aGT%;(`-6L-&##e53#,ZHvh%.F=m/(?M,#1>s1B.BGO-FTdT/HNLK(Y,<a*95$T.L>#G4OsJ7.&PKnM6CHf:DGrR<lA4[,>G0+*Np+u-f/iejgEx-G(DRc#.9*U-@o`u&QT<^%)G+,M"
        "+FaUmB?C#5DbxQEox>e4_k?##=dS2^/;5##6eKt%Yu)U/]t8ucB(xx4WE)B#Q&6AMP?kK4VK<1#VY)[$4BP##:-sV7^nNT%Es2a*r')L1?9:g(oD1l0RZ'B#l>k-$0(fC#QPpxFxx)ou"
        "7j^7Df6dV#e>&#,.rm@=wL'8[X>XKus-9q#c>vpL[?#&#+WID$MaIN+(c?X-6$mFuYZ]Z$K0@gLIl<g,))WUdOw;p7?%AR3@KFgC34c&#H5<a*r,Wt-57pfL0ZhlS$d<`skGpJVX:P>#"
        "nuGp=Wl*F3J,/B#9[3(NhBeNu_Qie0BZQ%+F)6m&l:BP8H0W784eWYJ>afX-mm+=$lfOaVx.L6tvL&=#$uTB#FnZ`Ee.0mf1WrV.wHF$'K6IW?5@k<:Q1Sl#36[@%x-PMVXEew9<0a<u"
        "oB<rN+?\?>#.#-'Z*5=a*`2kKPS.=`#Yiuhp8=E1'OMk9NQk[`E=nD.Q8?p5/'3G>#Z/MfL$gabP8s3]-2fVOu7+0:%,oeWVYa3.Qh?sU&jln51/DP##Zr^Y#[vSI$Djj<:QKf>#./EPA"
        "(`><-ee.i*#'Oa*ehqGMu[poLHfm)M*%HpL.'HUZ3R3[BcY3wp0*=$M>hd##1V>jL]#TfLKD?##aUwB&H5>##7B:t-4)XJMY5df_7<Gx7'K>6sa0c2MSOd$@l]'^#-bl*%m0L&#3L9U."
        "l06g):s]R.pcrU6u_NktRrCs-=B.58gtT#$vY4%P-R/>gN-hxXn[l;%4/<6.lN1GM00hxXu;*^#n;e&#iMJUD:))dG1I1N(vre#72($I:+RZ,MC^0m&Wq7jLCCdf_kY1m&**sY$=lMkO"
        "(Yao/WRgxX;l#^rwBGO-8gYO-7;HX$f#*a*b#%[-1Z9aYCJ7m-Ue'I?5$/t#?c/,Mn/ul]_GFL$I2Puu2wWx#[Nlv*/(fT.]sJI$K/A0)-J5-*qaT/14o0N(xO)ku&6LJ#tO[i.x`CL$"
        "ZU.Q-97hw/;Cno%^#v;/+)J^..Gc>#R)s`$S)+*$.=UB#s####FHCK(#(Qv$3W>na>IxK>J7X%$jL8j-ff7EGE$HUZ_$m&-QW2;Z()>j1wFx<(M9dtLGFVY9Cdv`-S8>L#;rtXVP+@p."
        "8*w$Fp:0XC3,'p7bmB#$K)?p'HP2T9;%iv#X:OcM/]qr$W($v#/o_v#BJG?#_>&t^xP:6&+/###Ujlj'UKgQ&=R<p%Sm2M%n8wD*s'U*IFBEbNcPqhp->/XV>cH/s3PsU6<5SDt>gqxL"
        "PsJfLHIQ>#s3q-%`l68%O$b&-2`Cv#U1OcM&&###2:J=-F^iA.MI;/:ICck=tT$0<][?Q%Q:A5#3f1$#Un7lLEiH##^NcY#+V1Z#`0.iC#tvv$/Yqr$Ap)OsR[Q##RZBU%brl29n2EtC"
        "?_(c#JA3+08*w$Fio1kuP7Ie/=I9SeF+,mf,G7(5OqAW$2]L;$Y.?;$+VC;$hSbr_YX>c->9+6LA8xiL5DE7[wHjNu7nDD%6Y$cVpZuU6A`k&#q/[Cs,N8RE.])20=.j;$*AcY#X<G>#"
        "7G1#?toimL,&Mm&V$sG3DK1I?Z(3SVDm[+%mIN89xd'B#sdCxLm2w##mk4[$/fU;$gVPE7>O-29Is>J?W#;99@?oC<gJ8gLoJ2XM'o>T%(+h,?SR3r$FIk,27w0XV.$o/_91:GDLocf_"
        "i<7b*=)ou.7`LYG;-U'Ak&t58l<a9DAJm]M^VQ##W*V>nYqvY#V6>##.YU;$jx+$+(O&q7ZN?g).q3L#9<N`#2CA5u`#6V#4RbQ%,oeWV[5t$kj?vXN[,`.&_>G>#_/5+%&)###<xk+D"
        "3`Lv#Z,coLX'Nv#5dqV$#mKpENVa;.YAfn$[-PQuhTPA=]=u$$wSJ1')SD%bgo-%%O4n0#?6[Cs#-Po'>jTv7<(iv#W7XcM+Auu#,Pc##,iNg(51(-MH.Oe$-$pT.xJ:l0;)c*]TEe*."
        "'+J3M)[eNu3I-,$`ob>%/a?T-s]^W$0C$gL8XR%+]v`Qs:a*Z5IWn^$,S1v#_KG>#(Dc>#F+@9%5V(Z#Z;CPMYq)Z#6NcY#hm,u8ac?g)k#R^$/9Wb$Zqopt'tqnuj_8c$3C1XVdr#Q8"
        "xlB#$FW<dMh,/;%88OcM/SUV$Vu^Y#/iCZ#CJG?#;:e8%))i5/9p.-#*SRGVW+x9.BJ0dM45fS%`ojr$0.ChLp1)m$nFV_V.1NIU(Dq;-]g0h$K:A5#)G6/v=)xD*f``6MN$*iMq@sU6"
        "i;jm.s?cuuEshx$[nQ##?4i$#+6-^7D+Bm&+AcY#e/@8%e&9.$*Gc>#,A,##^hYN':@Rs$e9]b%ffsPA<=3T%Gw^h(FFO,2vO9F*v14.)^8`K(^pY3'IcU<-1apW$]XN/sU00`&PJP>u"
        "r$p%8D-uZ./DsU6-VSs7f4b8T%a,t$2ca,2,[Ux$*Guu#V0,##)DP##IYdk9Aa2nUUjE+.L_4]:Cc0^#[>O,%<8NDtF>/C9dtg>$H?q[/Ep+X7Yf1Z#<uK?$(2>>#T&(5M#?pO&KU6hL"
        "vrSc$a`4itr&gc$@/1KV$(#V6+2PKWX=U5%=QMY5H:wu#`55+%'&F'#;uk+D2V1Z#LV=o$d5FE$-Y1Z#%HG##6@I<$?WC0E@UUp7>X0N(ej8s7hElN`Suto$(elq98-gq']*3$#eF8$$"
        "/)fA#CF-[$*T:v>4r-W$+T%_f?.r+;*/6g)ABmD-oMIZ85/fp&qYB/%eBh-%H2G>#QM8v$'Dds-n$ln*/.5-;v@va5OsL#8&N_'upcBI%x-PMVpZuU6B'E>#V8m&Q`WtG%)DkcM$,>>#"
        "-u`,%VRvu#i>%O;5b6g)/-kM(lX)<-tpI#/7w0XVG?rB8':]qTsq^@%@>r,N->G##@m)a*QAL_O0f`A.lVcf_7bEs-K)ixL=(sb;k@C>#>qjJ:vBNpBNYaS:Jp''#00,##fRc+[Fa'9."
        "pVJuP`[*@I(BbV$Yc8^4dC/,)&.d,4?4I8%_)KI$)AcY#es:Z#:GS&#(mu8.3;ba$sc<Z%m^(,)3Dpr-Ca=59fo)ac&=uM(eVoGM@4EuPKxYp7<B@gWJ1$##0A-W-;D[wKYk@a57]u##"
        "u8``*7^DY-Q#2^Qd)]e-wX]?pDRZk%W#%g(QmT,;R5J`tiEj;-3C^0%HHCa<q&%?$VGYY<+qX42u9N*%9^U?-aZC9&Y$MT@(nlS/ch7d*wJ56AnF1H2]]WI)SwC.3x?7f3XQ:;?Aw>>0"
        "A@XQZAt5>0ACbQZK%cF,K=b)JK(lF,+n=K$1hi.M=7QwL^G8jO7%%M&gl68%jYiu5MOw>#+]hV$?+6C%-V:;$Q)C8IQML</gP@Y%k/wD*c([5/$9L#$cuZ'=)_ZJVlE[j)Vi;;?$1i._"
        "&Vj''THfm':Qt>7Kr6aIr#$Z$ej?1D.o68%FRU,D)5###1atr$aMUpLFQ4D<=u^/&m>G]R1n6g)FxF)4,ZB)4)f;9.$d<`si?XkO]EM1g6+RrSiJ6?6FTF;HZjfBnqtDY7</I:23Y1v#"
        ";11,D2MlY#'Buu>C*_=og,oa.Hax9.+LE/1Vl*F3a(WUdK9VN#PG5s-M9_sMx%)cDU7^&#)mBMWw8T8%#VftB`B_`7TpG>#Fw3x#(JV8'ARNT%Lniw&IQUg(SW(O'c/$4*W9gM'357w>"
        "53+Q'faF_&pEq[.OGG##=lZW.d]-<.#NnL%<7?c*?9U<jKm6XLTZ3r7.g%I*cx#J*%m:Z-:JeC#'($Z$@q$`&^TIg)8'1Ui&c^)N>H$T-wu)'NtN.U.A*;juR_sfL[**MM;%73#`32#P"
        "hs*$#B4i$#IJ(Q(Nhws$4`(Z#x5x>#0iCZ#,GcY#HA=o&LG:;$NmqfLNCNk'KnNT%9:[8%)Guu#U=@b%1pvC#*wf@#(?[L(USoKYc+rZ#OA>F#AOfqu+_d:HYbM[TEsjrhbsHUurmV:i"
        "0IpUP>KVW$O.Zi9q%4O'%'];Q;>JW$LBRe-_*8F%S5<PAn5Bm'#on#1dJ%L(M'tp%7.@s$3RF6/4uj%$4<3jL&i*P('ce?TV*wtk3(ppu$@`<UQNWbk#u7nhc_Vo%ZH_TuW^o:#rh,q7"
        "+EQv$i+[w'.w7IG&&Z&#qj9'#>0$^7H;*t.#DUp$LId(+#;=.)c,)O'O'bT%(p.K<W-jv#j2Ig<jHx8%I40K/o7<9/w2xVQu-oD*uX#`4Ib(T/toc'&?DJD*h%/@'DENX(OrnU/W@$HF"
        "-:DD5(:V`5)__OEKGTh57SiCuSf^OE>>]OE,w(:OEc(,HEH',HR^aU-_(IM0aaTOE<9FD5^e1*M+VcS.c]WOELx*7/]?``5$*5>>Z$FA%kSdY>G/Ru]H/->>;+FA%^nHg)5&AZ>TYbK3"
        "h45cMpcfK3Z/h]=&unj4WjNZ)qf,W-58E0?rwugFPRQ>#u#]>#6-0<-TNS/&KYqr$q<f;-/Dvt$;]GM9R:.$$Un%/1k[,gLEMAG;%/5##Ve`&.UvboLUbdY#09P>#ug%##oG'aNI@:t%"
        ">81<-c<aN&gFNP&n.Om1/PC;$*DY>#UZQ&+Or>X-UaR?nSXH##:$cJ(Y6kC&R98s7_p9B#p-'U#cLu`NS^=`sp<g&vf6w##TiLY7cGG>#1i-s$f0S?g]%@s$<)>;$)N1v>RBi>nK1$##"
        "KOh;?t+LWJ>uFc-R-beH<g5ju-r;`#(VG1Mx(P8M9:2=-Te&_')s1^5C3-_JKrw5/4oqV$l;4GM-`iS[v(So%tu5/1?eO-QbxF$K_%Q^OF>5,MBUH##^:4,MrP?##.p3&%^%Js%E<5g)"
        "Oj4W-hW0_J[f-U.;K0kutS)G-Rx6s$5p7hUw]vi&@QJtu9./g'-Qk;->_FS&R.35&DGg01]>%s$h_H>#KN%d#H56X$1.lxuA+iE%U7n8%1cEY.6Z9@#eZ6ElXX6##CjZ)*^oL;$',ae$"
        "j]Z`8:JP`8_*cm#,.'o0uJ7;6Wu=,M1wx>-BaXv-+O`f:Li0^#ZN4Z-G$kwTG/5##V#b73ctW(Po'%U7.5=b*uAG<-pjW8U+[A@#?.Ni-hcS=ML3b-;FbgYAc[uM(8odf_%GBV-kP1vA"
        "0oI-?3FiOB2>cL<mET_-=_MX:VCQ)N&T+mfY*AJ1>um>#[CkcM';P>#X3,##)<cu>%fk&#1T]Y#mwF)<i2gG3$S;e.URgxXJ&E`?em/I?/t9d;:-#sgL=uM(tDF8hR0,w$9#9uHfu(RW"
        "I.?N<]d0Z-8;nQW[:rZuvZ`G<$(/)t&/(R@[Hr&$0s`a-ejjD?F(sq%J:A5#'r]<.4*x9.#og>$(]h-%ee7Dt*%PQ#Qp#`-JO.R<c^80M=7hxX_/ST?sF9x9ixLs-e;j^>ki'B#n'un*"
        "2lZW-$[KMsr8P1'2-l<U'_fw#-.@DHrIQh-vqP429$/10[[-_ox0.>MuCuSRfQ6?7$j*mf]FFcM=W;;$8Z/U@I)Z:V9xVn<3<U9VirkIuZdBI%dB=18;0Hv$$DWF&/@]LMbIEH>`.r_A"
        ",=2S$]M&C1wdlF#W:4*$(7>##?;w*0.JY>#&<lu>/7%6/0sed#q$%,;5`7>g4*x9.:+$@$l(iNVp%4;Z$xQxk-H<h>O8P>#&*;0$EgRbMFdRN1.JSfL:%</:%;G<%]ho&$nR-6M1W]C%"
        "?OY'$>h2/-b/s#$;=<t$g#jS>QYV2im8xiL+wko$isw(82frMiSne+%UXNt7%JO]u1=B_-Vmww9*br`N9>-##sw=u?$Y.EH:5gD8j^K-QgBR8>CJbvKvvpptS)*j$tx*GM*.hxX?TZ^S"
        "l3hxX7uf.6Q$n<%^#v;/+8>##W&coLOTdY#d+IqLXXvY#65)=-x%3>(09/pAYe=Ppd8pJVFNVxkWdr>$dtA-ddZIkO>w>j0`Ifv$,;>##'9cu>/:L'#:DCW$*2,##$OH]%g;:U%t4rB#"
        "w#+aNenQAS()P9SLemFSo7QtLb/-e%>guM0(Dc>#0kO:$bm3h<)c1*4@f$##TE31#i*)Z5XK31#^IbcM0rh;$_#Is$_@vr$aW31#4H7/P@V.q$T&..$FMJ[8nM&:D3hDW%D(J`V6&5b@"
        "8$K&Za,c%&5w]&Zf<3$#0t?Y%8+7s$ebQ<_+XOC&S,<PAXx[fLlnx[%3GG##NVja*Vhp<-Spml$Kna5/CJOS7p&H9M2qC.&Qer;-;F]v$@_0?.`ogW7qDkD+K5:K*d)Yt$r.rB#p@hp7"
        "nlDW%:d%`Vw-Kt:`/H_@&k`g=F[uM(8q?V-=UNIUB/2R3b3n2B4ZlM(B*?v>$NYB$svWV$0^=)msT&f$wn)jL3+</:D@mJbJfta>odB#$3mc8.M];>$ch[s-,3ZtL6eP>BnX^;.(1`l%"
        "/<06tWuFoL$FH>#E2kf%FH9KMEFKYG)*n<h>iG29xDd;%)RVv-:,h;-m&M@8_9OpB5EEuP%3EuPIHugEH?`.&P(&oSEaJ*-;G`MN//$I:@Uu>I[CF5A@phP04gnC1;d^hYv-.HMhtE$#"
        "+_h=%0lqr$ZZU;$^4q>#(>G##&*,##CeBg11AaPA+S:;$s####,.)n8rliW%r.rB#HgfX-mYxfLZ7hxX#$p%8ba0^#F=bj$uN/8O*VQe%.=x+2<fZY#jDAqD;>qKGtD?h>Q8>##<,p>#"
        "8GWPA$&oP)=5&F-O2/b-_VI&.<2d5M0RUX%/GM39JVF&#S3vh%U1(X?-%'Z-WPKc$E?g8.GH-XVmu4dMH_Q5&)VtcM0wL&#7tL&#kIwM<=6V-MIU`E)#,O+3V:k(N0YCv#)<;,#(/5##"
        "h.IqLS5q]=W4n1ciCnG;@ckA#q*`]$A-xOVc2vX&;P2t-V%+GMxxSfLG6N4;x&(N(R25L#PI;/:B=G7[/s59.-dxxb^?ap7n^O&#;x`:%+UU$BEMfW9)/UU;K1j&-Jjo?M_gwZ8%=cNb"
        "`$S5;ndE$#)X_=%54Is$XHuY#]:ZV$rt1j94`X&#SwhDH,I$^(H0fX-eg_t%j1hMBC;f`tAt'=;5K-$v,_+QuZZ$S%*N;<%cxs^-hKuBA2Y4R*EHVR*?eh?%`XnXJYSqw-g#Je*Q42?5"
        "d$RP/pEIlt9F0K1IV%@:vQN`<K;*mAUQ]3MY6fX-Vdk-$oY>W-Rms0#e0?'#-JP>u$kKf%uj4g:o&@<.&Uw9.bbCxL4)@mS$DP##]6We$jA+42I-hxX6L@JM)#;E%#B=)N[teK->gI^#"
        "*G:4DtB.*-0'EB%QDuu#'05##pnLJ:TUX&#.M]<pf?Fg$Codf_]rrhp<*2;ZBo>N)RXH##6/hF$ls`_:?CIg)(I/+O;tDJ:i)8p&7sZh#420p7AWP'?vZnO(>j^.$*lT)N94B[:j<Dtg"
        "Pw%$&UDx>IYPUC6-.v=&[?ap7_QO&#UH^%'C.;J:,4:E#v@KW&I,>>#Ee?DEmJ0X8@c6gDKAk<8JOtx1.M@b<+GHJV/26W-c_F@RXH_T&NAcY#Z9,##+NagLC6n=&=$K0:N09Z-TE2f<"
        "Ldb]uuLAP<^8Hv$XJ?)%x5Ts-pW`i9Gn_?%RI1N(0;b>$fIvP8]$8DtlH9W$H,(W-uGft0U_;;$DH>M9';D^##K;R84[nK<n.?d;9NwdO?@eNuR4=_$=9WJMQOC]&)=j/:bviw.3Bab$"
        "A7CYV[Et?%)[>q9,1</:$(HuPJ$O/:1^c6LpT//N.<rS.NAqj9sTO&#]BOG%U8,##*BP##1ih;$Em)oV2R?#)G0x9.8&-9.Qc3XV3g#X-%2(iX&d*mfeVSt3NXw;-NK.F'XNICe_Lcm;"
        "6'.g)FUWp.$>LYGntG9_;JKE;4TGrB[^.c=Zsd9g1*'@:KcV'mh.O&+@a4jC&iNXAP)4#&66)#.cPCe;MRGrBdl3_$Y8]-#R)5EN5=YR%,O>M:A5Z;%E[9$G/V0$n?6v1%L,w#>.jl'Z"
        "VS@$8#T-F@A4X)<6](uT`],J4W1jf=&DYX='&q>$F%M(?o;N71Rm@RC@Mt<uAf-_OB#$>/Lglf=-`UU>]o1RW_DYp7q+*RWMm/gL23Kc$I'GZG?MbrT;+jY$Ne7Q8e1/s90$ld2:';(6"
        "aJI^$ZG`ZGFRuY-),-<-U?Cg%^M>q7kj:Y]L-hxXm5L<-Tx1Q&G;Wa<1WU;.k/LV:=I#_$sxU,;FD2r9:/Dm9V@mU9D^a^%`qJM'?H[V%nn/2'c*As$'jic)u9Ss$(o0N(vcMD3668WD"
        "-pp3>E1x+$:<6bZ_0Z?$qTB.S6[s>&Kf+T/A]U#&lh-##HO[s$@'TM'o>(l03,m]#No+9/p?Z)4U`W3M4/w/$CJ;6M<jxF-*+BK%0p_pLf_9W7E),##Iq=aZSLZ>#rD'aN@L7&4J>#G4"
        "X$nO(k@C8.7-pj-u0>_Au%+ttPnSN->pSq$GPf8/iL)_]%s^Y#0>jlAqlK#$(lP<-+=5K.w[R_#;-,-*`Y0i)2nV_]6V<7M:.)mMZ+_#$gj^(WO#()aB)sx+?0<J:bW^s$+AP>#smI>#"
        "rs[>#Ma$gL&LuO%UfU;$YK1v#-0Xa%d]B'%,S1v#aZ_Y$5doA,P$[K)NI7<$hp5;%(kZ0)d$nt-XAxF4_R(f)r8Lh)*)9f3x?7f3u/Qv$JZ;h%S6_b#=i(?u9sJ5<4IgB-ZX*u7pHbU8"
        ">hcL+KwD`+v,AS[$'2+rOd:#5CQCZ-#4CK2GB-;/,0Vo0)8(A?HO$*+L^Ja+b=$##$2YY#`,q#$'bc##'Y;ku%2G>#7oJ.$T_46#?:9#]h.`Z7;]cY#1M,##jD%4;5NVC?&s>>#C$jxF"
        "x8LW-(D-L,`[Af+w3KmC(5voLN@c(N-p]]?rP,##<%i;$@'TM'rG(l0uH^l8LNv)4`FYI)KG>c4Hl:Z#Pe8AS>Wlb+9d/du-V0ZAKu'1(DsKkLX'mD/X9Sl#J4XL<6rd$$$S`8]v+oD<"
        "&g`p)D,q8/'_gq#qI0hPPDJ=dig&gL>:BsLsbZ>#.[D8$aO3jL$@W$#XOrMMk^=`#&/>>#vDcY#u24Z#WoIfL/GVPMO@?>#lT_$'Y=<i*F=s<-C8%M.lVD<%stE3*[%9u]wm?Y1TN.L2"
        "h*YxbONaJ280a<%pp7o[SKDm1uCi.$c)P:vSm9/UBSY,;0R>#YW>OGi3ko5#gkUGi4tV'#=<S>--5S>-5@S>-nAS>-$aiiLA.bS$?6g2%%m:$#$DOn-R=Z9iw.4GM/@h?&Sf68%6uU?#"
        "*Q6LcC[^m/6G2T/>;gF49h%=7Sng9C'WT,;>4kx-Uw06191%##jqxA$_K''%ds%'.A]YLM7oL3(j:ffLV8qq'lp&eX,Gr8.MfU_%g99:%f%=_1KxF6Dv-(E#EgJlN&>4m1S;LKo>]C[6"
        "clE+--L$ns7Agl8S%(Z$]AJkkOmZW[MVP$6K>?;7JO<FI64:8oF`iYO.udT-QYG6.DM1EOQY`QTD94xp5Qg:H,0WQ.h%ko7`TnD=f`h2D^@AEMC<s]1O9aQ.f7xx4,Gev6>.bx-N/E6."
        "&W+'O,WBj(P1]`WHJ.,)C)hx=.OAw8%]BL(2+eS%-2>>#Bih;$(w7V's<6.#-;,##>7LcD1%.W$/T=;$=YBB%WPB]$F7J@#m0qJ)3Gf1(L[[W$:I@@#=no2DTCe8/?;Rv$irxF4`[C+*"
        ",,6J*2uw:/jfl8pt6G#%X?6Pfb;[Z.Nc_FE1jKe4`Tw)4e.9buwMsY7+GZ/*I80/h7qLO12K^Q21?+fWrl+'/`Iw]6@7];8@$r]$3Y4,M/SC/_0S^G<%E2p8`p?Z-gQiYP,Rv-.j/xfL"
        "]LN,#CHgSAJMO?$;-8>,F^j'ONwmMT+3M8.e>dnaohXrdI`RBY?7[g)UfHX7v9hTP=GP<8;TB[6GwY;/8j<$$)OJVmWa:2pn3U$.%FVd*uhlI2aFT,:GWe+iU]b22$Xc`uiAnM0Vw8Z7"
        "PPqUml/.,9Jfg9V.-U9VCG>AVPLQ29Jm$$$-bUCsg+['Sqg(E#8F9<M9o62p,Com(.0s$$kix+is+4,rUuou,Lnkp^1dCx7Hj'^#O<96/1SUV$_8g$JbpgY?B##^$Cf$t$8r$W$eUd>-"
        "Mf,##f./m0N*n(+5n.x,?81<-;OuA+TH]88V8Hv$Q:)t-q5g:841vGE/$6@7`WFp._hj>7/UI9i9R3I$<V-=7TQ?e2IflXZU>.(##)P:v>,T##Rn0Z5w.2jT2CCB#)[Vci?\?fppx`::%"
        "bbrv:h5'gLOIV@#,X_@-qW_@-H^_@-$vkp9rpB^#S8tw9^l-$vDNKY$%&;g$n3n8%JRD21gvXm#GR;O#5_+m#v$^c$;7.d[.P$##TKvo*fXo,2UUQp.h23AOg;''%C?qcY%:rwKjn/I*"
        "QRQuuX1aj3I9ViYTo,L/r/T.$fxHiLXXg5#.)4^#SSkK4>_F2$KQ0:#Q+vr[h.`Z7EfCv#==C,DhHZv$tw1/:b]J]$*$4Q/2HgM#t*)xt/Y3X$vW2PVt$H'M7l+=$C&kB-p&kB-8^W32"
        "Axh;$swUf:DNXrHV,<E*86iA42TPs.u&H>>&2)XG]R8etc8Mohv=fwLg%+@$BrSe$iJ%k9hP4U&8fU;$pa@>#(NLV?KQ.(#20,##wjuF#1`Y##A:@w#(K59.XkC+*;X$?5H4h('J?WV9"
        "T-:=HuxB8Ri#l=6cOE,+u?W3OQ2YY#f&C+$eVc>#'t&-8.OLk+5o?p.,ts8%<#gM^bfZw'xEs%kDj6p@S4gl^:-Ck0]hv%k:G3p@FM4;-xv4g)f',U.O[*p%7Vq-1IYi>$%xe1$5AEFL"
        "w[@m/jn[-%Nfh?%<SW#&V;uu#qB5b#_`SX#/.s4#bK%C8]/Hv$A[Qe%l6i--SCh--Dj@#YW2I,M$Rj)#R0D'#]qpUuaLw4v6'<+&7[[KW&vN$%@'A.*Fk`/:;E-_#c7=a,S;to0j;Bk2"
        "%%g+M+mVfun:<)#:4n8%M]:&NVrO4osZZB#Pb%j03)bo[%Ygg2$Zp@=Wpxi0U->>#u%$j0G>t16-l0YtXjQIMoCUP8];fT&n5:D3.OAw8R4&T.+ZI)$-q=`3:dL;$'),##9GV?#S/Zn&"
        "fj^:%,*lc%:IEj9BiUZ@I^i%OU]tM2%h(E#=VE#7Thj>7Na)m1QJO22rF[F%8gwB#%Kb]#Zt($#0jAKM'tCM:aI+6B%/>>#P/FcM@#pg%)VFE3L/Ur8nn/V8f.dm8h#D99wQ:*#$uTB#"
        "wVX`j8KJp.efQQ/2XNv#C2?(@H(ui_80S&vmLZY#(uK58wv2%$2+RS%=tLv#.1'o0QWFx#kH:&4iG(*SY]G/R.G$C#'vhI_UY9m;6G[wKJb'5oLWFj0jso<-;^^H'Bivs.4i2Q/-7pF#"
        "]I<2'Xr/5oR)vv=04G>#xZ7)$QDxK8<l13C$*VA+U._0#Hd8aJP:9.2aGXFr+Q?$=L3C'>`[+V^:lYN1C8RJLM?gi'6$t8.C1i;$F5)r.Bkfi'd@b8T@Yd8/;r,]%lp/[#Q[iVKPa)p)"
        "gE0[@SF2K:?m;W>Cv`.;CB?I+3I-,#&8P>#&Wg6$$)l.#_:G(+*lU88i?KQ(@]V$[2A6<K3&w-$Y1.&6Oo:D5hSL,?fQcJ>64>a6]Xw]6LwwlLwPjB%HA+87.OAw8%=%qTWUZ>#u7+p:"
        ",n4%PCbkA#GC@'&3Qng<v-D6([+O):mN-Z$`J1]&D3`j:-B7/Cx$7pp<nN6L_$,s/hhg#$':F&#mY3?%_;,##8u(v#5icY#vK,+#'>uu#sGo_OZ2ms.q<4=$Ha53DY9^:/qf0i)#DXI)"
        "ZPBs734x;6(-Wj<c68*?nAOGF<cVi:gQ1auE:jxl^[_E6od/G*u[Wq.d^w7R(kM8.t1*,)UJi/%&`/H+025##W)6-.?_^`*1GEW-3cw5h<T<H*]]WI)+2pb4w%c3FcM8f3*N8B+M^ag2"
        "F`$n(jmP^G')&^6_Je?.n#8*NL/Yi=L?9[9K2l:$DYal$7&ZT',eJTEc6$s(xQR$#,XZdb[Eo8%_ndw'n]_u$j(M,%_<w8%%+>>#xA<p[qlx%q4wr#v]9D4%^F,3%0t.>->,/>-FBDiL"
        "<dIc$S9l>%c?/r1Uv9B#AJ>P3SsD^+*VxwLTLoqL^^D?%h./N1i/h^#g_7Y>Qs'=QtsNwKD(sul@PHsH/ITV[GD$F7VQDJq?4(?,Bmv%+4<G:.mrqB#(k'N0diwg:j?GF#Is'2:AVBj("
        "%x'F7lKvE74hg5:8^v;%Y6*/:'ANk:1S0fud2'S%9$QBijOoEIr/.2pP&jcrVL-5/OpEq08w2C8pESj0(oRs$Bxn.Yfi%x>$Bdm)V4(j$puQ1+.-c'6,+%1##)P:viOE>cl?c5Ap)75^"
        "B^Tk+r&%H)o10)sBPJP^hfogL_/a$%rcK'%Cs.>-t%x782sB^#l&+$MYMH>#YVa-$_8:6;31biKMf]x=1?;cuUWKfLu2A8[#+)N9w)8I*$4BJLKSAx=O>3cu3f'F.BK=p.ht-)jb54Wo"
        "E<Wj9]@NW%9+i?#`9B@'[Hx8%u[39/pnF`#C)h2)7CkE6C0Z^$-+q*+F@@AqVCSi1j_)#MB>,x#w]7h$^ND<%7_ErLhNx8%Y(suLxqJfLDV)xLA_YgLjX?##_<V/C>KWhL8IGcM(5G>#"
        "be'W.:$cJ(&Z^8%FVE%$1h0Y`hY.d#xMS=#N&hDnC*%b4Td[i9Zh'K<t[i`55fu>#q),##/S(v#<BP##;7Is$>Nc##4x$<$))gRJfp;h))J>M9m(?,4*^jTJ,cV8&X]bD..UO2'-9rG)"
        "A)6<./*-a3A)WF3vd-oJ#RmiViiAqCnv2-OiU[@9/83%cVF_*>;P*%cU7:e=Gu86*5(Bs?cp>vSvFc,@l=Dj<^^W=&*6(23D#=-D5F+?.SdL*#d#iK4c&6&4d,6a34uLv#W-Ke$a@*T%"
        "SYB>#-Yu>#K_TFW+KH?$0iL;$4>kp73$g*%_WZ`*4DlN0qKlW8`D0J3@G6t78s7p&@F:g2.]WF3J29f3]Ah;.B0[g)lw4uH+DTv-;xDu7oQcFF2>]8Ln`'d)$1]M'DS35Ag,vVTJm=p7"
        "@d(B#-:T9Mf$:SkR3OR3irBW/Pa`22+Tn+Dv8@R0U7)Z#3:-g37KLu$LATD5AbAs$$L,+#+YU;$DL)%&*AYY#DF_cD0oqr$GUL,D3DP##Y8uoL[GQZ$8,5?#$ej%=W7PI)M+V#7d<(1;"
        "8(0.2`OE9/lW'q%Jd1,)T&&k(-s8+*5Z5<.uiWI)QK[+4`R^fL*c+E*QGg+4+G>c4msHd)sU39/W6'?7r%MB#ct+^,P>/4(aV:>A&9il:3h&[#^=>%RiPk-mXw,[>g0d@pSR;fMPY^a-"
        "07@s0G2H=l.Xwr7I-l,2DFW?'L1NC,eqrZuu(aJFD&&YJfUn$6:^)V6]7WQEAqnd$Nk&(6D:Lf)c9_NE>RY&#*01KERQu)#1mS`7Z`8_#pA)V9<LAs$b2d`#[L%?7$j1U%A@<p%3%r;$"
        "GPG?#<4@8%Fk6dD<aJc$@bR`%`[j%=OL0q/Mf,##otH1<Sn*?@:hA&6A-ik1L+.t7`JG<-,n<o(aN'(k'Hq#$40_:%F_va$Mk9et?GUv->;gF47'Jr790KGN`K[D=eI#w%#epN:3[6)4"
        "+v8J3nMu',UuY03<+fL#3wrX-fH67*7w;(89S^VI0PODF6+H%$-_m;@EIMc>A]g-)>w*+N^S<gQFG_x&3(A(#dO>+#w=5C7[YL;$(;2v#Z.$^,Iu_v#qt.s$1`V^#*XeG;(J:;$m$^E#"
        "U3S8%$D&Q_KR9@8Xr7f3+:J@#Gs+F5^@2h39s(C,CK&i)l2(U+)01N0U>=c4MCn8%h?;58kX;<%aRx*3bMusoaZ>V/]]WI)ZE_G)Y[7s$+Hk8(wR$q;/'&X/bv)O'/UT88^.eS/xCVJ="
        "e'5Y0J+Gi(J7^T&*n.h@<T'wS7%+r.8p'xIDA+H=iVZ?$,>gpAa(6Yum)85BLMYWQk36SG7khP=k:lxt1e,N#q<=KYPkx;RTG'o/6*(wKmuv5-*J`u&Xg:D3pKYlAt/US%;F<T%<v%h<"
        "k^W?#6xh;$K2%s$<C<T%M_M[&MR(gLUX6##:H,##WImI4T]sj$`n[A6[V^f3aMt22*CP70U%Gd&EsLv[F-ot-hWKQ/P]B.*,%1N(R#G)4n:'a-vTNw6i)l-$L<Zx15%ABC.e;3(t?Gh1"
        "&Cc51K/Ap0XXI]6XNd+M[Il'2a####'w:(vJN.`$:qQK#nLEL#$T4m#kZ6I#Pe'W.A1r?#+s:T.C+i?#4/r5/A=.@#eu5tL-v*G#]oJE#1([0#=:r$#sc>N#Z05##CC7@#q6eW%-b$)*"
        "lp5,)L]#v#CqNP&=Rwo%C9[uP4+g(NHi:8%@B(,)xaRP/.8^=uRDp@k^k-AFGG8JC<W?PA8Cno%'E^oom`S?$RmR+i1b+;?lV8$$#cs@X0q'8@-FZ;%tDOA>@$@v$G9-Rg_p9]bE1Hi^"
        "+Q7VZ&6VuYilkW%7_JM'C$#2KKpS5''4LsH)73Z)FiJ&QRPRh([L1/#fSRh(gk32'nlE**nblR&)rANKWYRH)ah1/#iPIH)lE[0#Tg>7&_Fro%DRVZ#RTL;$g`VMK>K_G)S$I0#Dw*x#"
        "Z4;8%Jk[s$Z;3T%o.&NKDdqG)Y*I0#]N*1#BaLk'ue[qL9'*O'Gu4.#fYRL(_T^G.]p#R&D>$O(WRFG)`<9f+#/6kLC60;MObZn&8)&WSI**^Q8ZuJ)&`bHE7oq`?B*<VZ'3DYYq3k(W"
        "fUn+VQdeuP(E0;HW@u(EBjHPA1hOV?u=_c;c2JM9Qn9;6/'so.,Fa:mf$2P]&h^%XqVP]=iV=J:NR4>58A;D3,NBJ1uQ.5/CoII*AdZ`*#6xF4#dMD3SWLD3)+=m/.?:d2IU%],7Wh]4"
        "#DXI)@4[s$@f)T/#0Tv-h+39/vBo8%7_Z;%`uu3+l1NT/gBeb4s7WT/pcSv$e/p*%Zsxc3r23T/tlY1M:P5W-I>Bb.mnRs$k)_:%x#V;.9nd;%Ovra-D:wHHj;p3+h?QL20Gc05V/5J*"
        "ho^K-&-:w-Rx]fLm#Rl1-Pd'&V:i)4?0w)4+ZwS/)K-X.[un$$Y[Jj-YC-L,wAxF4^XgfL:[?<.6,;UDs1Hd30#hI*mrG)4R@AJ.=91#$UV&O=`)1wgm_t;.'o-lL9//rL*3+E*5.ow'"
        "vCQf*FA;W-u.3'o<lZx6Mf^I*pps^%g:CqDcPF/M7v9a#%Pxe2HY7Q--_,%.8J^kL#ef+4+=Q?gU`q*%3f^F*DB*<%1E(H2:i1g29e'R&G]d8/sSSIM6qNH>q^6B/X*9V't9q(>^>As?"
        "`QI/I>oh;$Ql]B+3?Cq%oxIp&79Kh*g$,j1X1C-<Av_B-^)dr%4e%0:51T9aLMqZ9)Cp<$hNUG3J_BQ&]7Sd%ONV8o?feB7lrwC-T#g)?SL:K6X]<qM(P^U#'+@`,Z4PT'hS6V%5rLZ#"
        "F+d>#n4ak'f<N`+_XHN)K>;8CW6:/%#aT%``k6o'PQQP1BL_x5raUg(oMMU%aA;,)u^:-2ujr_$[3%K)HA4>9ESKn;-d<5EY:r,+EcDb?QfEu$xr>]#=D2:%(KX-*d*36/ludr%93E$8"
        "<efj54a>o(Owj5&Eg$h(G+1/)_?8HPns[D$9**Q0SDA#,Vm7k(B/Q8/<v2d)8,-A/-Utp%$@h6_CjoY,1jocNfQFSEJ+mN%)L$^.l]4I#d;wd))gS:JdH&x6VIHj).MjO:8sVb3B>Z,2"
        "'SDZ@]Csj'k(RN:3+;+$6I$U;rd$h+)dnD*T?w.)73qm0#u9t%.SAI2e0]g:u<t;$lDh-2_[Np%RKbQ/Hi+`-&<Nb3DQh,)?)f[H]=[>#,[dg)vuV0(r[mIDf/W(.ncd5LCLsU.JDsB4"
        "BxC'+^/I*3kbti:CN7w#2b8o<N;8H)e#7e2UQ'^#/G/?5['Bo8vc[i16NQR&34nS%Tain'T,>H;COg?$ZqoV%C8hr.x6o;8u?sQ/Awdk([8G>#b&p[#iKZ6'&q&j0-]Sv$uMwu%eXBf)"
        "5];X.etYg)'h3>-]Xov$FJ#J*=@R<$q,;+=S6g5)mCd87-PAt%Wr_E4cnsU'.pZa3[#Op'iJ.l'KhG.)7+6C+HO>&>vXM@&.n]x,B+.<$>*rj(c=5@5&LXT%H152(_O`G+lL/9&H&>p8"
        "I)wt$1cB<&XB@29CD]l'xiDH2tmJt%]abp%'hhZ%:o<W%9:eS%Hijp%,pYt$pl1^#OG:;$24d%FTl*gLs@H8%(Zcc2cYnr6L]B.*^nRs$OUBg1QY6<.]CI8%2Q/.*rOiU8q)#H3q#qA="
        "GG1a4pKq6*5d5K)XiWI):@sB8,h'u$&G39/MweP/)5K+*CCi9.Fl?<.ak+^,-12X-?(Vt:(pK.*r81#$xmo8%3>XQ'%M,W-V<*I-H*`hLPo^C4f_1+*Y<'u$o7Lt:9o4R*$Y<L>X1+O4"
        "D%':)]IF<%m1f]42G]fLuT8f3N29f3N<a3Xw:6J*K%[2MX+#;/n+)a4n%oQ-V69oLJBVt-m8TkLqsYhL@'xe29r-lL50Rl1u[+E*GY^6ELVHh,ilXjL^oXS3OYGc4.;kp7fQ@;91`B2'"
        "CU70:$m0q/-Ma+385NAA<bP?(B1U+*VSBM)S_8m&9@%mOns_v5_&C'RGc.21Ei(v#O^uN'40_J5<%_XV95L)El]qY$d_ws$(,TJ3=D53)xwN`#Q/YE?Esnr/YKMJ)-C$<%E,%Q)HYw.+"
        "[efq/ufjX6ur%`=a,Qv$o.Np%:`W$64TUL12kYj'a]jl_oWV*%a==2CP37H)f.:^+YsRo'`[Z>#GX`Z#;:wS%n#?N'kChY,0vu<4E`*o&TeYfC,Q7Z?A$'ZP'sH59BxXu%8>cV-.aIA4"
        "Vwx_#dDng+8xk_.wpdQ'^[@<$c$quAHqkT%l/,]#_dDn'Hf)t.&mh11cDIi1>h(n'B3G/(.@01;v*oc2HI)E47'+#-KJGj)il<E*[uCV..<ik'cn)(IWYrSI-RH_+mEqGFSqm_5q))>$"
        "=FeS%F:FW%]HBB-'Z`K(etV;$59#x#e98@#u,lE,-Hu<8&HV8&imlV/bPCB#[^g`3A6N@,u8[)*x_5.)C1PQ'Zt%P)Cr*v8T,89&TM0i)3Mc>#6P[i1vvAo8[s'/D/nG7&Ud^M'eqvq7"
        "N<@b42sa9&CqNT%x,=G4C%X+3X:$S'1gPi*S&kp%@8PZ#ZF$9.[OQm8AhNT%*F]j0L1s^#t6As$A1RW$sYdg%3MT2#>@%%#g^''#9,3)#`O>+#14&W7i,/O(+Jl>#n+9I=K=[>#MNG'C"
        "_6dL<isvD*k^?d)FYMT/3'Cu$?]d8/@;T+4b)fF4e@Vt-Y:l/MZBSs$'v,J*^Oj*.)19:/BPwe28M4gL`Fq0M6$o5/spiI3APkI)/.VT.*T'i)HrjX%Ii3^,WggM'%[lg(K60U%Sv.`$"
        "]/U>$qS5v&tti20v2+?#vn-9.RSB=-V9b`<xVa]+<0^A=WdB#$Wl:v#c8oH)(5I`&jGj&+J0DG#47<5&^_%[uXd#r%$7Hk(Je39%U14W%;U8m&24'=%d7j[$vC.W$q7&L(7eFt$W>$$$"
        "^)Zn&dm]%#/3OQ'pvmu,+P4ekh8hP#Ko$S#s6'U#EZ2W#m1Yu#WQ_28D9E$+'N+L#ef/*#;L7%#7U+Q#bFgM#2>,F#1([0#<:r$#ChFQ#sU2uLf#>-;a$dG*?s0<-Co&D*cTx@k.RHfh"
        "?1G@%wk^-#J_R%##?8X#w1f-#1<G##`aoH;N3Q2(@CtT%[IVC?bhol&e)Hn&hNWe)eD)4'sBaE*0IYOKZind)o<I0#eGdR&i^aI)_QRr:Yg%e*-FcOKXV@H)J3GG<&NDO'Z3C5'&-mQW"
        ",[mUWxUTpr)AVhL(E5tV-;QOKcFke)Kl6i-'n(UV#^wd)?X;Z+Nlx[?J17K3/Y1v#R9F`aKb.G`tT,AX$rc=/BGY##x;6/(Ev?JLjc6<.V-v)4gYMT/mU83iVi_I;`np/)a)fb4IT*w$"
        "LnRs$==7lL&ukI);Y,<-bW,11OEIF4619:/B__F*)OMiLX[-lLc?wG*JA^+4GZc]u8MWs/#rKk;JS>/M.Stfs0H=[,WaT>$=&ZC+K$OT%OaXd;_$Ld29d]A$^R@W$SP220/=U]5aJdj'"
        ";hW)<&T'?-&O_G)x<jq./;3p%qp#Z$ZZQI)oYm3'Eu8o2)4qo.jxOv,Qamn&Mc(v#Td-@R06)cu5=u01XD@H)@C4V/mvkg1.F^p&;u?8%%lf]$L)a'4qr)w$]%4m'j[H5/7j_;?ps^m&"
        "dZn7RP$cJL5Vuu#j0.5/PvMk'UKgQ&9+wo%r-@G;*7R20TiB>#E2'##Um&F.ku6s$wDQJ(bUqo.TX)P(QRH>#uX4I)=Iw8%mDRl$71$G4iMLP#PA+@5dQ/i^AfpU#LXL._>uIfcrwaaY"
        "V=AT%I@(b$p8:EOW%L;:V;6*C$0k:H5[rCN3K<J5PUT2DU*qB#vxauCXIVS%EoCJ:xk]PA)BXV$pM6A4Qws?#x9@G;$),##^^C;$.]L;$X($?#XH*1#he#C+2x]L(lMr0(#jhl04NAX-"
        "*oi[Gs'h)3aD29/X$nO(vAD'Q_C$?uNO.p%Hh<Jf3YAbe*U^4A``dLCxs>oRGA)V?dt$J#CSJ(M>AI##ir1K$ndj)#v2h'#WC,+#C;Fn:_BpS%#93G7>@[s$^<[0#)Jl>#@%LF$3GY>#"
        "2o1?#BD:8%0gLv>hGav%<,BP8e]^f3&VjT/G&Tk2.w$H3xPCl0J#fF4'^B.*%60I$I0ihLnM(u$Y@h8._%NT/W1Dd3IR7lLFLuM(k>Uv-^2(@-egp40d(4I)kHL,3_pp<1^kY)4xd>_/"
        "2I]@#Cv)/5XJm52*$_V.*f&)=;-_h1b@fT%m()hre1Am*nZB-4>d(X.ZSM^#Rw[_u(agoSX/m`6.S@;HG4p.PS>+D3QotG<b':=OP4`S.j=q?9DdS3IqBZ_#pCw]->'Se$riZ&0-v&_i"
        "6flZ6%.0a<$&###agBSn4MYY#d$U>%dfG>#$UY%+)O<%=GZLfLAZ=Z-f;+F.nASY,.Ad>#s[+f,)-B@#IFH>#h?b9u3D#/hl4p$9_$(N$)(>uu?#%SuXD7#M0wi?%J/P_&:-sbNk/)=O"
        "ke?>#K66A+G*$QASU)20q.]w'PDY>#<&g>#R<GeY@4K2`sl7d)NawiLiXdf_$JrY>Q#%)QQ.q4tk=.2.MY*CNn.gfLNE'W7[kl-M9xZk*]WDn/gMr0(sVLl0tH*nLXEA<$N';2MW[tPA"
        "I%'#QR<mxuiE9C-Z2H-.0U_XO.s6AO^/3$#.=,)#-xVxJBp1P-9m1P-6-Xu*gAL9.IkNT%fE`;%N_$<%J7ZHNL;MhLl'=j.1`wSu%J00/6pWCE1C6xg?h9D3@rZY#*DcY#Tl#v#/]U;$"
        "=k')25uh;$-a_;$7pXV$SQonLjLH5.%3*JM/Q1]$XMpcNQ>WQ.q@BR`id8xp`l#KWD%@J$f/?Z$irC$#O%JV%O44R-A;&#+^9G6/t$/s$pIfo@9Q2<%?/'J3JpT=7E`AwDE'?6Cw*.7N"
        "IEq;CM`8u8V0IqC)N:;?uuPYl2RN3(E=.H+7PP>#IOlk'GLkUIM9?v$R*f)4rc6<.xmD#5kK7H-F/]VP$,oZ]?P(/Xl'j4LimD>O%(IM0*F]5#2RDS$5>q-M_%C6&+7S-GO%ew-Ljrs."
        "+_D.3kHuH?OMX%?dU5<.q?K,/uLMt:x>tZ^5c]'A^MUfNNn`U1F*?6C#4IRN(Ol-#N4eAePg`rZ^L*D@:$qCMv_gC-k-ZpLhe`rZJjAU)FQWt(E)P:v2>O#$?SR2.DF:28Z&>R*d]ke#"
        "d/w(#6w->Ojb?>#fD_2'cq7T%o%fr6XY61%o78d/nr^s&aX6w^I7d*.aBo(<&BSq7rX6_#%),##olBW&D-W&Fh&76irkf[>ovCT%<B]iFA%Hc4asO+H#&>uuWo`w$cvxE%6:w8%d%5>#"
        "/B<p['4/5uba(kLX`$##18v92:c7UAeI1=#R4>wLW`L%kb`UaMRj`H0H@R%uHsEx%7;Mt-Z.=GMM%g+Mes=T%uw(##=G/X,.RNa<hkAb.(1d;%lXjU-o5T;-neQ8.l3ZS[vIO&#b@:@-"
        "]sT;-$D,LNt[8+MNIqV-8)>_/4nS/$h5a>$Gk5F%F73F%5?k-$Pjr0$)KB_/GZCPg2fj-$,o'hL&^tH$;KlDNCdk%=lFnY6=T=_A38)s@k%^1$p`R+M.9-)MOQ^?NU<.&M.PT;-J9P9R"
        "qFG)8svrc<U(>Raua&g2`>=s%mu2v6^sE/2iQ$#QGkg.$]Ft&$r/qDM0v.>-;bPhLXk,fCutnbVZ3,)ODY'#vPNR>MgK,VdW@gl^fhfcNd<0,NmZ;,<V?CAPg%LMUT>BJ`qQ4/MRb+RE"
        "Y$6R*k):MqKQM0$Re)/-?Rbw'Mm$2h7xs-$1O`VIG-Z/$Aw2:D@XcA#K9ew'^v;;Rcc]1$vFp[O9Z_@-jGw^O%ekB-XfF?-5aGDNr]$<-)KwA-QfT;-HA;=-TgT;-h`E_OQPwtLk#K20"
        "beYS[3*RS[wan-$K,l-$00k-$9Kk-$4:lA#>N#<-i5T;-:5T;-,N-v.<oQWAovx>-F]_JXw0NM-RiAN-`[#<-7JwA-lQG58Yu&:)4Ca=-*M^s$&B'<-h%`Y8xLF&#Lp_lg.(A_86nQpT"
        "TC9IC6?g1'+SkA#0r.>-@7T;-bZ@+%uAik4[Uw<$.[pV-A=9R*kjV&m;Lbo&F(t9)7CI3)@;Y$M28T;-o,[:&Nqe_/'=)X&rS#oL[*E.N8d7iLa<*mLELKw8?pD:27[Wk+7+Lk+wWj-$"
        "L^#e+?'crL9Dw:9n<*ZQ%J'`spYOk4n$k_&Ku[w'U(kp^p/BnLr5KnLCPFjLoT)xL=8X+N-oHlL&8T;-FFlDNqLY@Mjv3+NMMntL^6W.NdhUHOo+.>%o3L&#p.6Fe'`aw'#k8.$Ml]_A"
        "V*mS/+.9w^Gj^w0-ha2MeR%n9'+J`t.8hY?s8;FR(t,KE?5u[&$Po2;_*E.N'o*J-PgG<-8CZ&%.C/F%.LI,3&0t;-,M#<-Q,M*%A&-F%@bn-$jIiK*9X+rL6KI*N?RGgL*NJ#MQQ]BN"
        ")ibS9ZSkxu+A;=-dhG<-36T;-fwH6%e;1F%0lYS[Ui,aOZ4;c`UjgreRpxc3R$f<-Q(m<-OStO'p3p2iS74R->@S>-3(G?-S=o3%`8#<-d(m<-(L?.%,xu^o>_NYd+]f%uEPuoJaCJ88"
        "XSaYQ(s#N9Drx2D7Gl$M&Z*;M%ft3N[[-lLB3urLL74R-kM#<-16T;-51n6%^,HdO`=0vLB<UoMG7Q:vRWaJ2_hnpTpA^nL9w(hL43+,8;OO&#>SA(%@$0F%VMS_&SG4_J2Y4R*,>+Fe"
        "7+k&Q_D`c)3ls-$/cM_Sr+W_Jf,/KawsVxLSjg&:mUkxur8s'%(YLp7-SYSJB^SEN=NJ@9t'2R<n/Yw0o*('$e`BK-3I[#%=;95T1krS&6@A_8-WC_&hfi&QC>+jL)S.2NVt.>-2H$i$"
        "d#.BN_xoC8Xj9#vDY+2vhhG<-06T;-HCX,%0bepK%%g+Mx[8+M&Ju%9/J,8f2M-W-Ee$'?+^S%9</wuQ'<t;-xQCx$WGk>n$wG`-6dpEIAarxudN#<-/XjfLeSDrZm'L&#49/>mo<C2:"
        ",0wrR$M+Auc2p;-2E[#%H^Hk4Dd23;HbCH-Pl,t$`+.F%ltj_&$.aq0jL,;-N'mxutM#<-)6T;-*5T;-DC*<;SnCT3>8?&MaqO,/`gVS[5Qo-$qGp-$;_#:)$dp-$qdeVI')-v?dS>)4"
        "XPq]5r2o`=,owlB#f0B#+1D:v=xdV@t=O&#R9dxuYeIJVEa>AYi<DJ`(Kio]C>>8JS$GDOFX1p/DFP8/Q1pxu6^n'&*k3$MJqOrLiIhkLUh?lL6bbnMpBf(<d<0,Nd`OPT[lmY6]E6Au"
        "P/OJ-n)m<-t+Ve$i1Wk:oLbxu]eL>Hrr^G3V>:&5fZQ]uffG<-@mB-NB&]o@%/T$1<(trL%ho#MuxYoLf]JC8vro(k`#wuQ;sk;-JN#<-w6T;-*6T;-2XjfLn(k=cGWS5'3A'/`,2q;-"
        "aN#<-ULqS-:gb4%@F2q'x^8KN:/P[&^=B?7x)J:N.(`e9/c>Ga@5B-M'd<uL*]8+M&Z=;N5%W^8Qa45T=sk;-@Ns'%`V_QsdZe_8Qd;R*kVKF.$Tl7$gLT0$HS<_oU5(KE8)1-;QhcR*"
        "4xM2r=Qef&W<83i'Zx8.djVS[vIO&#gU)>MZv`#9Ap]G3A1r;-FN#<-2a/,M]%buLcIBvL2Ac$MWgV>9Ed?>QhlOPTOAl;-;O#<-L6T;-/Fx/MNDT;-1L:@-KO#<-.b+p7eQX]Yhegl^"
        "`QmA#8g3vLY*m+Dl,[F%SA%x'@<3jLu%buLUqvhM<1ro.YAu`4FX1p/^(N;7e0s;-u28)s5UcRe>*dv-&SB%t_3&8oQYl;-wO#<-p7T;-P7T;-l6T;-=7T;-'nB%%S)c_/6l>La?2A(s"
        "uv8?%xwSfLa/;]$H3T,;`g]q)7.lA#Qq-A-r6T;-9ZlS.t=/5u2b/,MVxfD$2D)uLmn#wL/b;xL>&tS$*w`'M-w(hL7`:%Mv5KnLwbgvL)af#MIF@&MNTZB$AW.nL9DSb-E;1F%.pNwB"
        "%df--#AYS[&_Lk+<_n9;u2SS['P<:)%IR#MJfipLO:.&MJtYt9GRbA#/,oA#3egW$Qbf(M?obP$CQEwTh;.+Mv[PT:.9o%u98-INK,@tL;fjfLgu((a1G3Gs/%P&#G2^gL1_&fL?V[A,"
        "P2uQN*p4I:W:>F%+[Fk=kS?FI43M&#biJM_1ku%ueX_@-TViiL+xSfLfu.>-.7D9.p?ZS[uCF&#wsv_&I36,EhUw--tPP9`^l1.$Qv<OOU+F$8hcg#(9,9kODu5K30af#MeiJ+Mrx^(M"
        "Q9X'ME9MhLb3=(MY<Kuc<b^>-3Tt;-3%9gLY&co7kbZ-HaB_`/`hSF.;wX9iSpwD$BGP)MBjkjL9kFrLD#<C$Q0@V8(_3>d2fj-$M:Ucj+QR8/<%r;-gF[p-pp:R*SR@.$$nt92'mp-$"
        "b#2(-rT)xLNpZ5NH%+n-h0qS*IxD'Mp2]jLTSBi^kLt+jBsk;-ba,t$#PF&#sbdfV.o9Wf.c>W-@_^q^+$[xO3ls-$1L/F%n3eq'/qugL^*elLEM>gL59MhLe9`F$srmiLFbq.=Z;Lk+"
        "$BCpLSgEr7G#[w'Y@R<$I&^9VeE$O$#jLC$(EbM$B@jP$i;$iLOIWK$BAoY--=s92-5?.$LG-F%[8ip9vqJfLBbN.h$AbCsO8>R*kN4F%Dmk-$Yc:.$oX#`&$>o%ud?:@-UHxn$pZ=oL"
        "RcpV-&DLpp0RArm(JR9iE]Aq^nET_-TqoQWL$`q0h@TY,7e5,EeiW&#.q8gL/_oRnHbr%c&$vY-=&l-6:5j%uplVGsk6joto'l%u(E9W-sTbw':.'Hs):uxqG=/5u5bBGsvtFk/ux28t"
        "#JUYrZ,`csm)xU.comrs=FoY-oK6qTM21[-r.8R*HF5R*S^0F%*8w+sn3^gL9Y=Z-EDrE@'`aw'wYp-$(OVS[S-/F%vau9)Pwo&-GVOjLu`YgLjuIYOL97I-#DT;-<]q-%D'<(mn&rOf"
        "qiMcsPSaJi<dG2'or3)F80i`FF37`&:#X,MPx0l81NO]uXPj3&atxF.7Onm86kkA#U:fr%/q>:;:=Y1OAYqm8OPQfrQtG&#i#N:2B3c:2Ed3dtS_;;$j:t$MEDq-8ITkxuuvI;'$40K1"
        "j9ZS[$`TS[),XS[3c:WA*K7p-'08.Q.+hdMMvK]=HGF,MN2W7<%&)(ZLp$tLAKK%:/oVG<GAl;-=D8v$f]XS[%HG_&TG.x'@+dWJw$g+M'Q(>$dZXgLmiGp8@WtxYqop;-SM#<-AgF?-"
        "=g;+&I.8?N=nLq<`sklT/PNcs;kUcj)Lp9;%SkA#hpko$hDOwK/&jwKJ<Ok+EL'.$V4-:)=X+_]H^&:)++8/M-)-W-V)YF%k8#<->TG-MIw3'M6C;=-q+Q5MgiG<-:_o3%fH6R*QK=.$"
        "W]4gL%Q*20e]YD4m[DGWXPq]5%0mr[r#t]O=tcW-gV7R*D=5s.2fj-$IYA%8XNO&#rN#<-K;.FN[9lc-xHp&$XaTM'&VO&#$#-D-GV(AMs5l?-hEJ3%qkRF%c51F%pj9Ra^%.F%Nh_9V"
        ";LlxuIFU5%f%pJaWh'S-8gG<-^e'S-VN;=-Vas(*1[adXwt]+M2Vo(<Z#GJ12(,F%N0QGE5<t;-oT22%0<o9iU;4'm$@c(N[qGfCji)5g52oxu0h#<-XX`=-j%i9M<PC%kG3o;-WB?xL"
        "K:R1gfeH-d4f,.?7F<Q]Xf;;?K:gA+vZQ_&Hb<S*QHON9gwV'QMBVIMV7E#QmUG-MF]RA->Zlo$UE;B#KhFm#*GpV-M`aWS>=T,MaTw^OSp=Z-Y'Lm4H_B.$CYF#129ckOE$QSjoKaEe"
        "XC<m+f<sFnX%/_]:=c&#sIL>V.0s+;1APk4x%Ab8H?pq.8DXS[<6kd+u&3(8ZHsEIdSsEIxj9UR;o5i(Wr2dbw5UE)rIJ-v=[%##d&k-$1kVS[0:r-$Clq-$<Tk-$[k@.$NAn3ij/#0N"
        "dNEmL43DhLGg]K:XQbA#]%.g);q5,EFZCPg@#B;IoIKk=8CX-?SFr-$3:n-$@bn-$13k-$3E9.$OP-F%Ovl_&>_%##uf/.-tlIfLSDetLWv^(M*WoG$)+P:ve)4A#u0CJ#ZM@@#X'AnL"
        "#S.C#0p'hL%t`J#aw4Y##$+A#r$1J#Jfr=-::eM0#`6K#'=,F#t(rxLot4T#3cr0vUjjf1p-U(s=0WCjV$IJ:Sue+MjRA8#J,x)vB]*CN1J_i#]O*vM'FII-M5T;-(6o/Mv*p+MG*9-#"
        "u<9sLY+kuLrgDxL4Mu$ML3O'M?DX<#-Yl##s,niLVnHlL8fC%M+wL:#^#t1##K<jLZ0nlLslGoL5RxqLM8RtL@I[3#<U.%#`^[#M@@7&MNSHt.j9%##]Xm-$9Ln-$Q?o-$j2p-$,&q-$"
        "Doq-$/6+##M*.e-wf,F%GTNW%GlQS%R5T;-k5T;--6T;-HQP8.Vwmo%om@s%om@s%4$&9&v(35&4nV8&4nV8&*Q]8&*Q]8&*Q]8&*Q]8&0#PT&61NP&KVjfLA_YgLhC:as?QC0(omrc<"
        "1FTJD</N9`xhuwLmFA8#8YTkLO^g5#30CwK4mC_&/@pE@/@pE@/@pE@lt.%#M(^vL=3S>-V/1[-K<K_&wXQ2()4E3([_+/(uhFJ(2A$N(do8gL25DhL#(>=$d-LhLmF2119Zw<Z81r%c"
        "P`ScjlJQJrf9_hL4V4I`H.(kbIH3<#<PUw0qZvE70i@k=)#h9;c%n92c%n92c%n92c%n92e,E,#/UFrLo^#`-gV1F%,&q-$Doq-$06+##nNj-$6Bk-$>CfD+?IoD+c2^gL2m@iL2m@iL"
        "B3d%59DDc<</N9``fp=#8`WEevedER'UamL-G;pL(Z]#M@@7&MX&h(M=3S>-W3S>-W3S>-W3S>-ZNO;.`,'##ASw],M7=#-Yln#-a=ou,bXjfL(<P2(lM1p/8a6w^8&/kXd^_sLfC9vL"
        "7g(P-d_CH-d_CH-/vgK-.qdT-rV_@-+X?T-+X?T-scQX.VQ&##&2RA-2%kB-XeRU.VG###tLXR-tLXR-+on'M1Fel/u:9p/-d3p/+.Qq/V8dl/pA)203KDM04ATm0<T`i0tI5m0+q2m0"
        ";RQ21;RQ21h/fM1:SgM1bukM1iH[j10o[f1xVjfL`2CkLM.-XR(q9>Z;C7&c*,KkL]8LkL^>UkL4?UkLv>UkL>D_kLSC_kL*D_kLtC_kLuIhkLcD+4UloReaXR->muC4&uwxY@-?hN>d"
        "/J#lL%^-lL);Ar8mkrLDIu62L>?s9):_eER42F,#7NM:#O4(=#w:9_8#>>jL_0nlLslGoL5RxqL#T+1#?CE_&`CE_&`CE_&`]P1#En]w'0:o92n[$REcG3_Ak#s:#Y_L=#I,3hL$L8j-"
        "sb3F%;Z+##TU^C-TU^C-X$v[.oE&##sX`=-TKWU-b[X(0oC)##17*##uX`=-rIVX-.KH_&rJp-$];V88Tt:T.W.BP83VjfL.n$*5s#sc<c/Fk434r=#SVWhLCY@Q-0d[m-QXF_&TO)##"
        "s;8F-s;8F-QvgK-b+NM-b+NM-%m+G-=R[I->R[I-/XkV.cg+##]Yvh-rA-F%_fl-$`e9m9Jp8gLbIHBZ@;N9`@-1R3u]A;#U[c9Dw_c9DcTBk=cTBk=cTBk=1q+4#Z-%RE6*^&M7^e;#"
        "EELfL6W%iLN=UkLg#0nL-x.qLE^_sL^C9vLHq*'M7v3<#JaqfL:pIiLRU$lLk;TnLAKx>-5U=Z-?dH_&$dp-$e**##$<8F-7Z@Q-7Z@Q-8d[m-$?w9)UKo-$n>p-$02q-$L1r-$8N+##"
        "Shj-$Aen-$YWo-$rJp-$4>q-$.Ewf;/N<,<3ga,<<:n(<ocA,<ocA,<=%C,<=%C,<kVA,<q(5H<)C3D<6WjfL.PpnLRPpnL)T#oL'i%].I6A7L@$KVeskNw9Mh6kOMh6kO3&k0,Xwk^o"
        "p>Z-?'_^w''_^w'W9..#:,(_SAbr=#eK%RER^JlL9X`=-0Y`=-]cEB-P`&sOslGoLmnGoLItPoLmuPoL$KMP2RsboL7L(j*i7ZuoQe+8hs6FPrV51pLYM%^RXACpLcuWv88Hu^]0/t:#"
        "h$LwB2%LwBE'ZEe.pN:#E]>mLD_CH-(1QD-C*MP-q86L-UK-X..x%##T01[->X0F%f&p-$QC)##/fn-$L1r-$X`r2CAWo.CJWjfLIx.qL<#/qLvx.qLT#/qLY_Ww&ar6qLV(8qL>p%F6"
        "+:-R;<I@&cQ8r-,_a.R3`0vQNE>8R*nw+7#WU2nL'[AN-x%kB-TXkV.fS$##'>9C-A)m<-41QD-tX?T-YeF?-Lh(P-We^o/B8)##8l%##b'l?-AgG<-@]AN-Rm+G-?N#<-^b0s.d7*##"
        "TLo-$CrK>HwecYHQW3ZHOOXrHrurcsU,h;(1voE@E30F%3=X9VC#bE[)Z/.#V=Jk4>+;0#U0&kkLaG_&n:w4#'MsfL2?VhLRlmt.L$###>An-$3-tlK/4a2L-Ti.LqGYML^o$NLY_.JL"
        "A#7w^FC;0#a[Jk4Z[R9`R9Q&#Sj:p.t/$##gNl-$XmpfM,I=gMx5b(N`00,N`00,N`00,Nx*OGNkfvGN$>'DNSmYGN$ju8.SIB`Nu'kcNbFscNsfG<-:WjfLTQwtLPQwtLZcvb@0Q2uL"
        ".^3uLL_mb[1W;uL[F9V2q81oa3dMuL+)t:MiHD/#B[;w^%5t^fegMwBh-S9`so:3#+'4nLw.moL5eRU.I7+##SV^C-6NXR-'Th`.Y^(##&5S>-a`NnLaGLlSD.KlShf45T98TPT)jxPT"
        "W?,MT6>,QT2JGiTGVjfLqUF'0u,''^81r%cP`ScjlJQJrCo4wLf,?wLc$X?UDu=wL*+?wL'KsW`F+PwL_9QwLNwCqWG1YwLW?ZwLDEk3PP/%2hXR->mgS^-6&xJ)#c_?kF,W-kbc8rQW"
        "E3fQj-K8<#7$+_S`mY9VHAxQN1qQk+$_v:#hTKrLK44R-hV^C-Pq-A-Pq-A-(*m<-<HuG-G64R-XFtJ-vIvD-$o7^.9j)##1PYO-h>9C-b(Wm.e9(##Y?EY.7U###X)LS-lND*/>j)##"
        "/)o-$YWo-$@Eds[>eHo[Wnd4]>,38]8r88]pqOS]6'vS]S,El],Wio]9//5^&N75^U.05^)[LP^ak>m^eO]._&WjfLSt+$M-/+h@`m3$M+,>$M#,>$MbFd6>b#F$Mp6P$MG],7,knct3"
        "n>%_SU%D_/qp+_SYDIwKRTpEI7`2F%^h)##mYvE@0KJxL.lP<-Wm6a.x7+##8N8j-u32F%<Vq-$BC;Db_B<DbFO;DbFO;DbLw.abG^2]bNWjfLp`:%Mp`:%Mrm_[iuPC2qE`Ji%8a6w^"
        "Yo,F.?QR:##T=w^9H_-?\?D11#)#8hLX/66#P.a6#*$5qL3NYO-[5S>-af^o/RI###h0$##`5S>-^C3+0sP*##L/&##bY,11/,+##gx&##l.'##mfF?-#Y_@-4eEB-;53U-/BqS-s4_W."
        "c`&##1P9g-ep3F%%m%/i$k(/ig;I/iRiR+i2MS/itrnFiR5T;-c5T;-*WjfLn9X'Mn9X'Mn9X'M)%KwW*.W'Mq:X'M>]<_V+4a'M8Hk'M#Lt'MsMt'MmKt'M5?#x<-@s'M_==FLfYooS"
        "N%&F.mao%MDRh`.A<%##44RA-aMC-/=.(##G8k-$K,b&lrcGxkDnc=l=Ws]lxu(Yl-uY]l&p8gL,jNS$NUvFU$VlQWlW*RECaW.#aj]w01MhQj_YS:#+1]9V&IDjL['LS-rs.>-X2QD-"
        "ORZL-#M7m-2c<R*Cs+##kFSb-kO-F%Bvoum)N#<-=6T;-cWjfL'5$)MO5$)MUTi.p8-,)M@;-)MUlaS?MiuoJrc<wT^seE[k<i9D9-<kOV-6kXnJM)#DggpLM8RtLrgDxL4`O_.RP+##"
        "i%jE-i%jE-j./b-P)_w'iLdrniLdrniLdrniLdrniLdrnjU)8o+G-8oDL[8oNiW4o-6T;-_WjfL1h=/g=uHwBk$QwBk$QwB2:(,#bJw^foHi9D]R;-vt^&%MS(Xj.n=###qFSb-?h.F%"
        "lh`oolh`oomniooUK,hLi^d)Mx_d)Mx_d)Mjs$<G>Qc)Mi^d)MaSwG_>Qc)MT1F<#6Znm.g$C2:5D1_AFEruL,Z]#ML3O'M;)MP-p:x#/31$##&Pl-$p3flp5N#<-I6T;-b6T;-1XjfL"
        "*tt;lAd(*MZx2*M=e9HqCp:*Mk,E*MF3N*MX.&nnF,V*M9BO<uY*InR2(,F%OVC$#>7q/#CA1R<KT@-#XNQwBSVlERSQYEn;ES=#00B-mi_$ktW617#c0_hL%^.#/oa%##b*n-$Eqn-$"
        "cgQ]u$O#<-EXjfL3qS+MFu]+Mou]+M8w]+M9E$>>=i`=c@#sx+$q^>ZbJ7;6-x<2CYNL&P>am`*ff=pS3O/m&@pa>HU+ofLr6]`a'4,#YAq)aESeMm/u7Ads?X$HDAg?H)j@HJV$'QV["
        "N%&F.%&49#9/8-#:x.R3Ka<jL%9m`-bq.F%_J'T&QN#<-f6T;-(7T;-@7T;-[RP8.-Csl&lxBp&NM#<-c5T;-%6T;-A6T;-kWjfL%fcgLed:Tn5C[<$>fW>-%_(R<,KloLff^g-J&1F%"
        "gmW5'Kt`5'X6`T.#USM'3VjfL]+B$5s#sc<$VlQWU?-tLGY@Q-$_#`-2IJ_&TIr-$Cr_m'C[oi'L.Dm'po8gLm,?$G-9@k=Qm=_/Gj?-d$,)R<NK&F.2v-(#W`g9;J'e--J'e--J'e--"
        "J'e--ne%_SYV###%*%iLBT=Z-[L.F%(wsM(AlqM(SJlM(sSpM(sSpM(sSpM(#l2j(#)v/)T3LG)$E(h)d>hc)'H-)*_6O,*EAQ,*ZfG<-k5T;--6T;-RWjfL]x4=Z<=r%cP`ScjmPZJr"
        "Gs:o&B4pV.ZcQ>6s;4&>5klcEMCNJMfr02U(Kio]@$KVeXR->mxUO&uQr&J)lZ6iLHk@iLHk@iLIqIiLIqIiLIqIiLIqIiLIqIiLIqIiLIqIiLIqIiLIqIiLIqIiLIqIiLJwRiLtvRiL"
        "`vRiL<Y+>HomQiL[vRiLBwRiL.wRiLX&]iLX&]iLX&]iLX&]iL+']iL]&]iL%)]iL&/fiL,d,p8r)niLdDnV@t5*jL[<XVnu;3jLMK=jLF*Y>dj&0?-'9mV[xMNjL/#?po#TWjLwcbjL"
        "@8=KVih*'GgQ.e*A[xQE->9-#s#j/#im<mLDpQ+#BJ)kbpr>_/B1*R<3NwE7IM;'#Rx>_/Ks4-v=GPw96&*7#AqSqLU%wX.4sB>#7I6p-ln3F%@pF>#sd^o/%@E>#?5C>#72RA-tV*//"
        "ssA>#?hj-$uV:#6TRW>6MH#?6G]nr6oW9v6@3u;7dc387[$T;7/JR;7<u[;7cJ,hLSD3mLBE3mL%I<mL%I<mL%I<mL%I<mL%I<mL%I<mL%I<mL%I<mL&OEmL&OEmL&OEmL&OEmL&OEmL"
        "&OEmL&OEmL&OEmL'UNmL'UNmL'UNmL'UNmL'UNmL'UNmLodR)c6UXZ.toQ3q),lN)VinZRm+T*>F'AnL$?h6_ZwRC6NW4oL)9ci*_-/,5QYAv/;7r8([PUpLTB_vf4:FEZP/`-,RI=_/"
        "3n=pLnM2NXUiEuL4u,wL@@7&M/E@;#A)'lL]]Y%0r5C>#/qF>#AA;=-r^BK-9rO,/G[A>#utj-$[9$HEBF6DEDHvGE(RQ`EAE;dEiXm%FP,&EFAb2AF5XjfLMKrF?l[<rL+f=rL.nFrL"
        "PsOrL)#YrLA#YrL)*crLU-lrLJI5/Gs0'sL/@1sLkA1sLR^hSMt60sL/G)/uu<9sL<F:sL&xvljj?4kOA`[-?O,`w']A>kFbnh--EgI-Zin0+#_RJk4bP>kFXG`w'lp*=#0fsfLTj*J-"
        "gr.>-,pBo-4Waw'g3E>#C7k-$#*:,N6p8gL&LntLoLntLBLntLu*P1#o+unE#-jI-Hc:pJ1T2uLt]3uL2^3uL1f<uLld<uLlfh=:f&I%BbMooSE*:kF`OP:#cHk5#Np,kb]J2(#biYw0"
        "uLV-HOPj8#I0S9`fjMwBAuhER-(x4#GmMwB=K>-m*nU+#H0jP/-f<SRJoWoR)E<8SBF_8SCxs4SVw:8S/m/TS4+9PSjZlS.)>p1T;VjfLKT0?L0&+2_h&:2qA`ovLP<.'9wG4&>-x<2C"
        "HF?-d<fL##tpeQjIOrlL%/moL(aDE-)eRjN;)MP-hIvD-0Mx>-6*Xj.;E?>#0X*//4-?>#_An-$(tniUpYT/Vwrh+V+/:/VBp8gLN2HwLl1HwLHd.qj'Dscs.KIX)NKaJ2_1jV7m6i^o"
        ":Mu=#[qx9)JDJuLM&l?-36kfLIrk'KsRD4#X,H9i&pf9DI>o,#MdnxLbn8Z.K^?>#%*m<-P.OJ-b96L-0i(P-0i(P-0i(P-0i(P-UAqS-^4J/0PhB>#aBC>#A%Us.0ZD>#c+k-$('F>#"
        "0PYO-?HuG-?CrP-ro,D-_KwA-6Z`=-`KwA-8gRU.D)E>#^3RA-DOei.2:>>#`)LS-c3RA-gRgc.`fE>#71]^.Kw>>##RZL-vJ,[.n4F>#jFtJ-ag'S-cq-A-J0PG-;%jE-Y(l?-==Ov/"
        "r,@>#1j@>#n?EY.0fF>#VMx>->sO,/u*C>#c4p-$(pp-$Doq-$wOH&cEwr=c-&hAck*8Yc[VjfL>#`%MZ*i%M(/r%MaD'j.r+$&MA5%&MB;.&MdA7&M?A7&MQ+SPd?dlus2>;j7QhfcN"
        "P9o^f?.A'#;#MrLpc&V-&4RA-cCrP-3sdT-_s.>-`s.>-`s.>-`s.>-eAFV.A^@>#k[@Q-p#iH-hTh`.ek>>#ZB;=-<:6L-V%jE-apnq.vCB>#5jj-$k`aGjqF9`j.PXcj$LQdj5Yp@k"
        "ea5]k7VjfL%`9(Mdxd_`2[A(MleB(MKlK(MXrT(MbqT(MGsT(MMx^(Mxv^(MC'h(MTbMxj7$p(M75$)Mr3$)M@4$)Ms2$)M)3$)MBH:GhSo;Gq=Nu`MKt+T6omrc<YZ?>Q)-@k=fR9_A"
        "Acf-6_c`-?K%[%#qr6R3hkd#MY%wX.g,A>#=Y_@-B(wX.sDA>#$hRU.XMF>#ohG<-6&Us.UAF>#dPk-$_ks.r:`@/r;vqFr:Z:Jr0>@JrR2^gLl2N*M?9F<uH5`*M)>a*MFch0K/QmV["
        "I;i*M4n'19j(12URI=_/:*F_/:*F_/;-F_/]`'/#a/'%M8l+G-qJvD-8ZkV.DNE>#-[`=-DH*b.$;>>#1o+G-FQE'/Uw?>#2No-$,3F>#p9Af.v*D>#]l5d.FDB>#ILB0/'@`Y#xfk-$"
        "OI`Y#G(m-$g0aY#>&^Y#.=%q.gGbY#WNj-$6Bk-$N5l-$3Ih>$-N#<-A6T;-U6T;-j6T;-(7T;-<7T;-P7T;-&qE/1%']Y#9d]Y#L@ZY#5;8F-a_FO1WXZY#f/bY#`o^Y#V4m-$GaAp&"
        "Zp8gLc4VTRw1vmSF(RH`AKPn&1FTJD/,F2_d':hL`-;hL?4DhLIBN$Yf3LhLlt-bs@7KU@MsiEIG:k)#L+L-Q=s_'#_7f8#SGM7#b%h9;`TCwKI1t9)`O1wgkU/R3u0a$#vmpE@P_5R*"
        ">]<#-7`/?-)ZbV-a*(Z-Vp8gL;?+jLp?+jLo382h:*L2UMsiEIM%]w'TZe;#@9lrL6s3<#nIh9;O+]w'$/9-#_=kuL$B8#Mqw49#m80nL2IVX-G%3F%o2Wp/LDvl/%RQ50k-v50,N;20"
        "E^R50bp@Q0*VVM0NWjfL1rtjL&x'kL$v'kLdDP3Ln-O'G:^&@6Xi;@$-8TkLrp7(#/H,Li/DgkL+QqkLfM5f*1P#lL&]-lLd50M)3]5lL9e6lL`U`r&_3a@Z5iGlL5oHlLjoHlL;qHlL"
        "kuQlL@wQlLJ&[lL7,elLK,elL02nlLY1nlLi4F5C;7)mLm<*mLG=*mL:Fe51<=2mLBhHAmpVQJr=C;mLgH<mLi;0B->IDmL'UNmL7UNmLgVNmLL[WmLL[WmL[]WmLNKVgEBbimL3u&nL"
        ";u&nLtAjN;Et.nLA$0nLK&0nLk'dZeMcBh3Bd4R*Ro9_87/asLD.PG-BW_@-4g3j..sYY#bd&V-T%,s/`X[Y#xK]Y#_FT_-Q(/F%Y9%h;9=ec;=QED<IPP8.tYa`<1asc<rwuc<<-wc<"
        "6s&d<$B;=-,7T;-ORP8.pd&&=^H[D=#Ir`=R-w`=?'[w'40G0$SQo##tSP4#cr3hLEdF?-#N#<-#N#<-DuQ&/9o`Y#QAm-$qOQ>?mJpY?j@mY?*,hY?/MQv?*^qr?^#5v?E04v?2K0v?"
        "JQt;@tg68@Y-4W@ipQS@JO*s@2])s@Y4Rs@+/35AaEO8Awp8gLfVUjEk'(EQ^Y_pLHa`pLVfipLEOUja_`hpLAnrpLB<bvo7MkE$QhfcNauwjk$-,R<+Q[9#Zu]Qsg%b-6>gv9):B_w'"
        "=YQ9`G]Ok+tPx.#Ydj9;lL#F7fM>$#1JmEIpl(_SJtB-d?fB;#(1(iLk;TnLIv-tL(Z]#M2Ze;#FR%kk+1&oLQ8RtL,s+$Ma]BK-)QZL-CM.U.T1aY#?)m<-mJ8r/s2_Y#;2`Y#NfRU."
        "fIaY#;,NM-I5`T.PX]Y#<8Af.f?^Y#>,NM-cZ@Q-.9M&0XgYY#2K_Y#n:7I-WY`=-n54R-eLx>-<cP[.L4]Y#d4S>-p'l?-LN#<->QZL->^Me./)[Y#<ND*/wUaY#J[k-$-'TML$cVML"
        "EwsiLEwsiLEwsiL(vuiL7bf/M>&x+M_WjfLv8RtLrABbI,3QtLrlTU)e)*=Cauwjkj^[<#R)AjLEdF?-WgG<-)ZKk.Id_Y#`75O-/l*J-m/PG-Mo,D-Q=D].Z6YY#2q-A-0spk.>5[Y#"
        "ELo/07sZY#kc`Y#5q-A-5q-A-.L-X.qo`Y#pm+G-So,D-?KwA-,(l?-Bf2m.MUbY#@FtJ-?AqS-2@:@-=.OJ-IwgK-l<8F-ur.>-bN#<-3;7I-k5`T.<e^Y#KdEB-Xs[B0l@]Y#k>`Y#"
        "F]j-$%fm-$rB%QTsJ>MT::QPT?[;mT-UYiT#dX2Ux^u.U,v=2Usp8gLeZipjEu4wLm&6wL$3R3>MiuoJ4<u^]ik,:#&H'kk+=KrLM2_W..tYY#u5`T.DcaY#<(l?-f.;x.52aY#Vuj-$"
        "]1DgV;@n(WKVjfLnDdwLoJ[3uKClwL4S'4PJ>Lk+W$+_Su)7_ABhM9iWun^oX'+_SP4Q-Q:g+F.JL4R3:g+F.T'f$#si+F.ab1wLpVJn.W)[Y#p,NM-lvre.qr[Y#]_BK-c&kB-AaCH-"
        "J@:@-NS[I-*1]^.u$cY#X3RA-xDsM-EobZ-WKaw'=mbY#-*m<-jND*/2W`Y#pCk-$kCsuZ-Ja;[V_$8[/NZ;[=j?S[qbSV[WmUV[<wpr[AGas[lrZo[t&w4]k[48][4OS]Q$$T]Y.<P]"
        "DJjo]#I:p]J7Wl]Lj25^@E25^^F05^wBLP^N+pP^DI8M^7A4m^>QSi^Krfl^42+2_K/S2_Z[o._fwGM_+uKM_rEHM_L3ii_^v@j_rwk+`2WjfL%V)7#e/O$Mi+&C:`qH>mf5X$Mm<Y$M"
        "@P:[M&V+&ug;b$MZcQ*phAk$MROu$M`Nu$MUS(%MNY1%MVa:%MK64])wg[75kxuE78rroLYsEf->50F%V5g`b=q8gL#Fqts*a>i7Y4Z9MJAk;#O#Mk4n#Mk4h0[kLT97I-1a#`-q3I_&"
        "i%Z]cI(%^c0/AYch/,^cT9]uchVjfL.#`%M.#`%M/)i%MF?/8>qxg%MR*i%Ml0r%MFl^uas.$&Mo4%&Mp:.&Mb<.&M$P>8P8vEDUc/Fk4hv0'MT^Nb.d6ZY#>-NM-7hi*/MK`Y#1,k-$"
        "J)l-$OYmretqnre+Fxre%K,hL]]Kj.wFH&MJ0Q,K@],^V#SZ&MmF8-'&fv&MTq*'Mix3'MlS'(MdeB(M`lK(MxPQ)M(SQ)MN<9T?@Wc)MUen;Ym8C2qrctm%>fW>-99.&G-QW-?@B<kO"
        "bgM-Z>YI5#/?$8#[w,iLDk6a.DpaY#'o+G-LLwA-G]bKM,#jE-/PJ87q:>>#oaYMp6DWlAP=75&hxWS7,4$>P&4>>YI(vu#vUk&#5tLrZ)(CB#0>###UVp-$vxe+MQ:-##JXldM((^fL"
        "Vf$=NnL?##WYM>NVRH##$Dt(NWXQ##>gS,NX_Z##_X@/NYed##rpd2NZkm##39m=N[qv##Jj(.N^'3$#mIU9N2ecgL@r6=Na<a?#Y9l@Ng1+Q'^?*Z#+/G]FcgXoIT1PcDH]4GDcM8;-"
        "0m>G238V`33ov'&X.E_&e&hx=FtXMC'[cP9box]GfTf'&fvG2C/C;=-UPE#.*<eqL6.]QMqht)#BN#<-4OD&.6sarLouBKM<.lrLxF;/#e,KkL.4JqL*.lrL)XM/#_9q'#RqX?-HMM=-"
        "AN#<-fKL@-m6kf-C^oqD[:P50R:X>-]b:_J6<T-HEE]3FoD0eQCe9A4C1vJ26*W?^xfFe-73al8KUku5IYpM1$?U=Rocf>-:ZV6MPT=RM8'HDOWj7CTqT>OME(m.#5WjU->Bg;-,H5s-"
        "7mWrLF.MP-JUYO-FSrt-&$@qL3v_x7O,o34+JPcDi-7L#V2Ys-3aErL.xXrLl-lrL#(m.#*@EU-JPwA-Pc%Y-n.Y'A,5Ye-[NlMC'exK>c?h+MJ`4rLWkFrL9Ag-#ofeBMZ`4rLWQG&#"
        "0%]NM;HgnLNLoqL=-lrLfKG&#g]mL-a'l?-ZV:d-Fw3^Exw%;H73.&G&gZd4Ak'T&63f%F(]fh-W#4GDU,Ps-[Y<rLNND/#oZ=oL5.AqLA@1sLp#YrLFU0H%[Bx?08<_w'P)uKGe2s.C"
        "e.cYH[W>2CIve]GEvE>H$P2T.k1g*##;Mt-hr&SM:HG&#sL^J;=8K21JJN^61`7(#-Us-$U3<p72gxFI]:2<-3Km-.&HwqL-lrpL0(8qL5@1sLXU[I-]kl-MSit)#,MM=-HBrP-(#Pp7"
        "PW;Q1oQ;/#N)1/#*aErLV'crLY(crL=RxqL<e=rLoR#.#$ConL5YLX8PhNV3v(+GM8ivIU5iHb%uQj-$/1d;%&N5h1h>eS#B)78#4AfP#X+ofL37<)[e_dV$o?.5/wxFuu98(s$RCHP/"
        "l1dS%URdl/>`?5&MUf:dPM$m&RY?2'[k)20Ff+`sD<5/(a0ai0Ja1,)ecMG)[Ooc)k+f4fCh,PfR;ID*jWAJ1ld]f1npx+2]x&#,a:^Y,6+EG2u*$v,g_YV-W;xr-btUj'G1A8.]>s+D"
        "rA]S.0^8VQOpi7[]Y?D3Eq]`3+*SP/wj320/HOM0Y^.]t'90/13GR]4-^,,21vcc20JX.h,AtIhcB;fh7D``3;]@A4?x3>5A(xx4C7XY5E@ku5K&vOortZ+`IXKV6ITBrmO*QS7SB258"
        "WZil8[sIM9`5+/:&QNcD.s/DEO(l(EfY',;TIh%FggEG;Ym@M9#)5##61`c;e;]i9_bDD<n[u.::O[`<=uCYG?k<A=>nW]=m`=J:h$H%t&4SY>r%u+;,XOV?GLU+iHalr?ggx7IJX@VH"
        "43ho@8KHPA'wclA)`q(<@&aiB-K%/CFJ]fCp`GulpFm%=LoXcDP1:DE_eU`EDbWoIcp9PJp>u4JXbQ]F6ZoxFR3Mo[f1sLpxw.>>_0NYGC3KY>)D)VmAEOVH$`b%OHe*8Im/coILZ,;?"
        "tSCPJ%53JLv`_lJwl?MK%/w.L,_br?)GWfL-`8GM0wBS@APWcMfmq(N_]3DN?]S`N9R1AO=khxOA-IYPEE*;QI^arQMvASRQ8#5SUPYlSYi:MT^+r.UbCRfUf[3GVjtj(Wn6K`WrN,AX"
        "vgcxX$*DYY(B%;Z,Z[rZ0s<S[45t4]8MTl]<f5M^@(m._D@Mf_HX.G`Lqe(aP3F`aTK'AbXd^xb]&?Yca>v:deVVrdio7Sem1o4fqIOlfub0Mg#%h.h'=Hfh+U)Gi/n`(j30A`j7Hx@k"
        ";aXxk?#:YlC;q:mGSQrmKl2SnO.j4oSFJloW_+Mp],1JU_,`.q`9CfqdQ$GrhjZ(sl,<`spDs@tt]Sxtxu4Yu&2>##*DlY#.]L;$2u-s$67eS%:OE5&>h&m&B*^M'FB>/(JZuf(NsUG)"
        "R57)*VMn`*ZfNA+_(0#,c@gY,gXG;-kq(s-o3`S.sK@5/wdwl/%'XM0)?9/1-Wpf11pPG2522)39Ji`3=cIA4A%+#5E=bY5IUB;6Mn#s6Q0ZS7UH;58Yarl8^#SM9b;4/:fSkf:jlKG;"
        "n.-)<rFd`<v_DA=$x%#>(:]Y>,R=;?0ktr?4-US@8E65A<^mlA@vMMBD8//CHPffCLiFGDP+()ETC_`EX[?AF]tvxFa6WYGeN8;HigorHm)PSIqA15JuYhlJ#sHMK'5*/L+MafL/fAGM"
        "3(#)N7@Y`N;X:AO?qqxOC3RYPGK3;QKdjrQO&KSRS>,5SWVclS[oCMT`1%/UdI[fUhb<GVl$t(Wp<T`WtT5AXxmlxX&0MYY*H.;Z.aerZ2#FS[6;'5]:S^l]>l>M^B.v._FFVf_J_7G`"
        "Nwn(aR9O`aVQ0AbZjgxb_,HYccD);dg]`rdku@Seo7x4fsOXlfwh9Mg%+q.h)CQfh-[2Gi1ti(j56J`j9N+Ak=gbxkA)CYlEA$;mIYZrmMr;SnQ4s4oULSloYe4Mp^'l.qb?LfqfW-Gr"
        "jpd(sn2E`srJ&Atvc]xt$&>Yu(8G##,JuY#0cU;$4%7s$8=nS%<UN5&@n/m&D0gM'HHG/(La(g(P#`G)T;@)*XSw`*]lWA+a.9#,eFpY,i_P;-mw1s-q9iS.uQI5/#k*m/'-bM0+EB/1"
        "/^#g13vYG278;)3;Pr`3?iRA4C+4#5GCkY5K[K;6Ot,s6S6dS7WND58[g%m8`)]M9dA=/:hYtf:lrTG;p46)<tLm`<xeMA=&(/#>*@fY>.XF;?2q's?63_S@:K?5A>dvlAB&WMBF>8/C"
        "JVofCNoOGDR11)EVIh`EZbHAF_$*#Gc<aYGgTA;HkmxrHo/YSIsG:5Jw`qlJ%#RMK);3/L-SjfL1lJGM5.,)N9Fc`N=_CAOAw$#PE9[YPIQ<;QMjsrQQ,TSRUD55SY]llS^uLMTb7./U"
        "fOefUjhEGVn*')WrB^`WvZ>AX$tuxX(6VYY,N7;Z0gnrZ4)OS[8A05]veMiT>f,2^B(di^F@DJ_JX%,`Nq[c`R3=DaVKt%bZdT]b_&6>cc>mucgVMVdko.8eo1foesIFPfwb'2g%%_ig"
        ")=?Jh-Uv+i1nVci508Dj9Ho%k=aO]kA#1>lE;hulISHVmMl)8nQ.aonUFAPoY_x1p^wXipb9:JqfQq+rjjQcrn,3DsrDj%tv]J]t$v+>u(8cuu,Dc>#0]Cv#4u$W$87[8%<O<p%@hsP&"
        "D*T2'HB5j'LZlJ(PsL,)T5.d)XMeD*]fE&+a('^+e@^>,iX>v,mquV-q3V8.uK7p.#enP/''O20+?0j0/WgJ13pG,272)d2;J`D3?c@&4C%x]4G=X>5KU9v5OnpV6S0Q87WH2p7[aiP8"
        "`#J29d;+j9hSbJ:llB,;p.$d;tFZD<x_;&=&xr]=*:S>>.R4v>2kkV?6-L8@:E-p@>^dPABvD2BF8&jBIR+,)Ls_c)P5@D*TMw%+XfW]+SwYJCNi=,DR+ucDVCUDEZ[6&F_tm]Fc6N>G"
        "gN/vGkgfVHo)G8IsA(pIwY_PJ%s?2K)5wiK-MWJL1f8,M5(pcM9@PDN=X1&OAqh]OE3I>PIK*vPMdaVQQ&B8RU>#pRYVYPS^o:2Tb1riTfIRJUjb3,Vn$kcVr<KDW3?h`WmF/&Xol3cr"
        "q_f]X(0D>Y,H%vY0a[VZ4#=8[8;to[<STP]@l52^D.mi^8k%v#JRif_NkIG`R-+)aVEb`aZ^BAb_v##cSfMJ(eDvuci]VVdVcmi'fj18eq7ooejKZY,MJWuYQKdlf%uKMgId(DWtu&/h"
        "-Odfh4F8]bc042'1hDGi5*&)j_D#s$;Nx%k?gX]kC):>lGAqulKYQVmv<frmQ(NSnU@/5oRGDPo[e+2p`'cipd?CJqhW$,rlpZcrp2<DstJs%txcS]t&&5>u*>luu.Jl>#2cLv#6%.W$"
        ":=e8%>UEp%Bn&Q&F0^2'JH>j'NauJ(R#V,)V;7d)ZSnD*_lN&+c.0^+gFg>,Vd,/(Ove4]Q*i;-q-Ds-87(58wQ@p.%kwP/)-X20-E9j01^pJ1:[jl&7NZlALwGS75vP,2982d2=PiD3"
        "AiI&4E++^4ICb>5M[Bv5Qt#W6U6Z87YN;p7^grP813PxbH(Ll]:`,M^/Ev=Y71[(a=<]V$x$Q29fA4j9m/v@Xlf0g:p(hG;t@H)<xX)a<&r`A=*4A#>.LxY>2eX;?6':s?:?qS@>WQ5A"
        "Bp2mAF2jMBJJJ/CNc+gC^A48.D__GD2%g(N9R(AO3lj+MFg?PAXI_DE]b?&Fa$w]F,S%SnrfMfL^=vYGkaS;Ho#5sHs;lSIwSL5J%m-mJ)/eMK-GE/L1`&gL5x]GM9:>)N=Ru`NAkUAO"
        "E-7#PIEnYPM^N;QQv/sQU8gSRYPG5S^i(mSb+`MTfC@/Uj[wfUntWGVr69)WvNp`W$hPAX(*2#Y,BiYY0ZI;Z4s*sZ85bS[<MB5]@f#m]D(ZM^H@;/_LXrf_PqRG`T34)aXKk`a]dKAb"
        "a&-#ce>dYciVD;dmo%sdq1]SeuI=5f#ctlf'%UMg+=6/h/Umfh3nMGi70/)j;Hf`j?aFAkC#(#lG;_YlKS?;mOlvrmS.WSnWF85o[_olo`wOMpd91/qhQhfqljHGrp,*)stDa`sx]AAt"
        "&vxxt*8YYu.Jc##2]:Z#6uq;$:7Rs$>O3T%Bhj5&F*Km&JB,N'NZc/(RsCg(V5%H)ZM[)*_f<a*c(tA+g@T#,kX5Z,oql;-f`>c`Q11W-u?i8.#XIp.?Wo4S'q*Q/+3b20/KBj03d#K1"
        "7&Z,2;>;d2?VrD3CoR&4G14^4KIk>5ObKv5S$-W6W<d87[TDp7`m%Q8d/]29hG=j9l`tJ:pxT,;t:6d;xRmD<&lM&=*./^=.Ff>>2_Fv>wj5ipx#f._(<WuPdIpu,Mv8SR_q=]X)j?`W"
        "LfTrQ6w'W?:9_8@xgf.UwW/MTcq]%XvHNlSDv;mAH8sMBLPS/CG:/YcPi4gCT+lGDXCL)E][-aEatdAFe6E#GiN&ZGmg];Hq)>sHuAuSI#ZU5J's6mJ+5nMK/MN/L3f/gL7(gGM;@G)N"
        "?X(aNCq_AOG3@#PKKwYPOdW;QS&9sQW>pSR[VP5S`o1mSd1iMThII/Ulb*gUp$bGVt<B)WxT#aW&nYAX*0;#Y.HrYY2aR;Z6#4sZ:;kS[>SK5]Bl,m]F.dM^JFD/_N_%g_Rw[G`V9=)a"
        "ZQt`a_jTAbc,6#cgDmYck]M;dou.sds7fSewOF5f%i'mf)+_Mg-C?/h1[vfh5tVGi968)j=No`jAgOAkE)1#lIAhYlMYH;mQr)smU4aSnYLA5o^exlob'YMpf?:/qjWqfqnpQGrr23)s"
        "vJj`s2C-&t&pf]t*2G>u.J(vu2V(?#6o_v#:1@W$>Iw8%BbWp%F$9Q&J<p2'NTPj'Rm1K(V/i,)ZGId)_`*E*cxa&+g:B^+kR#?,okYv,s-;W-wEr8.%_Rp.)w3Q/-9k201QKj05j,K1"
        "9,d,2=DDd2A]%E3Eu[&4I7=^4MOt>5QhTv5U*6W6YBm87^ZMp7bs.Q8f5f29jMFj9nf'K:r(_,;v@?d;$YvD<(rV&=,48^=0Lo>>4eOv>8'1W?<?h8@@WHp@Dp)QAH2a2BLJAjBPcxJC"
        "T%Y,DX=:dD]UqDEanQ&Fe03^FiHj>GmaJvGq#,WHu;c8I#TCpI'm$QJ+/[2K/G<jK3`sJL7xS,M;:5dM?RlDNCkL&OG-.^OKEe>PO^EvPSv&WQW8^8R[P>pR`iuPSd+V2ThC7jTl[nJU"
        "ptN,Vt60dVxNgDW&hG&X**)^X.B`>Y2Z@vY6swVZ:5X8[>M9p[BfpP]F(Q2^J@2j^NXiJ_RqI,`V3+d`ZKbDa_dB&bc&$^bg>Z>c9A5`jI>x=P1hvFi(nj=l23S%k-hV(j8WOxkmcV;d"
        "q%8sdu=oSe#VO5f'o0mf+1hMg/IH/h3b)gh7$aGi;<A)j?Tx`jCmXAkG/:#lKGqYlO`Q;mSx2smW:jSn[RJ5o`k+mod-cMphEC/ql^$gqpvZGrt8<)sxPs`s&jSAt?>kdGl)(C%A6vsB"
        "%K$D%UxGhFjp^C%ohtsB(H?[&P%VeG%^4rCx.&B&Y_nFH[6iTCPdBm8BF9X1Cs<^Go;?)%Qra^-]pLU)rZbgNbwd29eVI59*P(T&kZ@:0x':oD>T<.3+$5u1MXsTBJJ20:K$M^#[gtJ:"
        "+>g;%]='C-d-Vt/ProoDem>[&qHd@.4EpG;3QNv$]dNrMxlM:8]_GtCtjbkM6;@IO#^J+4A/W7DdtYtCm)VOD%Lv<B:>-lEv+[D%=uQA/^CclE;QRnMW/M1No99?.=/.^=>9&T&a^',."
        "BUe1M?,879OY7#>TfOv$Y5@]-WZl<UA.wt7,]*@KM1k@KF]*@K&BLlM>YZ>>*cL0YlY4ns'K_lM?jw.O&Zav>tu1eG.`r*HZICeGlM?[&C@SY-oDmt()T_PM4Gfs?V/GW-`NPX(U&thF"
        ":RiO;4VJ^-44d<-M0SfM5#SfM07tN0irV$Hi0iTC#I_1NX#bN94QDeE.E(q1lYTL2s2$A'C)E?8xb@5Bg]H5Bc:u;%6$LhG<r80OoDZjB3Z8w$jJcdGm&pJCQdf;-;,vW-:LH.-7VN3N"
        "'D4nMZn%-D(u%U.oHZ:&h,*:/SpG<B=;g0O,b.dD*GGs%ci,h/mG]b#hD:dD1AsfD(;#w$,#_kE@-sM`a]xb459R]Buv`r&#HFVCpjM=BnKitBA>f?gMc8%8*:#H3YSwqL>:bAFO;>U&"
        "(/DiFBuViFhKb'%jaQT/KkQJCol)^Fkva5'Tc'p&#/d'SV7p@8]ae]GscJv$0;9?._S?vG>9&T&F]Af.o3CA$;0gr-VcHu13XnhM<b;:8DuQeHfI<fH_uQeHWF5&8[2?Z$L9`qL*wLGH"
        "nWrTC9ZEUDWsjUD]bcA8_],<I*G%'IALV2O5G7pI6XHv$$$@]-*ji;Sf-U2B[tsS2E8IQMQO>HMbvcW-b6%`8<ExP'KsrQMj6-3B9VX_&F1@WAA_cdM5'mW-xF?t_3n%T&M3P[MhNM,D"
        "VsS,38&ukFtpe68h[$1,O(p2`UbQLMK3[68d8nw'7Pw6M50V7'X49dEJ>boMq)]dDvqmG3.n2b$SAY2(]pI2.Y[PhOlWU68fKdt1A7TkN^);r&1cgGMTw?68Ukq$'1%)NV=RYdM<]4(P"
        ":JSFOriUaNE#1%Q=fOCPx@n^OK/WfN;SobO+B(q7;I]'/>6+<-5*=J-=?=J-HZRuLe2=J-@H=J-KdRuLx]RuL>3=J-I^RuLtVRuL)3=J-6*=J->?=J-IZRuLe2=J-AH=J-LdRuLx]RuL"
        ">3=J-J^RuLtVRuL1&m%0Ob3r1eF`b4O-G[$<IFmB*SvLFi9a&HkniU86t`s%-`nFH73ST&&$N=B94Y_&BO<&5'^E_&EO<&5HO<&5<-c*.EO<&5CXWA5'^E_&FXWA5IXWA5<-c*.FXWA5"
        ";l1g2L@e2MnnN^-PAK(8-I3IN<BbGN>hQL2NZ2'HGC8Z$Z&,<-,:ko8TpdL,><WDN5x@TIKUvlE3,HlEQs4X9;F)W%?d,<-PJ@Q/=LX$H]e%o'8rqk16pkrC81X$Hp,a]A+GsA&NATgL"
        "ueiX-(^6L#OE%oD`2ugCq:MZnkL3[AoF*k9fV0jCF]:l+53SMF&$^b4mlu8.>ERL2Wa&g2ocoB-Y&3i2x(-W-krQF%*Yd_/FE4iM*Cj.N.a]f3_#'g2]/>)4^paJ2)&cJ2kEnk42b<iM"
        "4^-i$)jYA5_vjJ2k`>W-)JIF%WuQF%>Zp/Nq#g/NG'D*N?u779m8>_ftm9FItm9FINuSkXo@f-d)1=:2Vh-F[Vh-F[>9K_SNuSkX4kEiMTxDIN0br2N&p).N7#EINVn9HN0<QD-t:f3N"
        "Tdm-NYcm-Njcm-Ncl2INv7d9Njcm-Nk+*M2BV)ha`SfF4W*]D4Oh[D4GO[D4*N^D4Rr_D4Pqw`4@@w`4Pqw`4@@w`4@@w`4BRWA5hlUA5:EMR<RnNR<RnNR<;hRhMEu]/NZt]/NYkAjM"
        "NfDu7@F/s7R^1s7_PIt78Pc(H*'omD.]bv'D-[w'25]UC,<)4=^E04=^E04=]?'4=Aag/NTqJjMXfDu76PJ88P_O88RntS8A70_-%vQF%/Zt;-?4F,%AhQvI#eov7>VfS8Y,p;-O?-.%"
        "MSFF7@4CRN@4CRN@4CRN@4CRN?.:RNCm#0N81#0N90^N20iJ59NjH59NjH59NjH59NjH59NjH59NjH59P#nP98SwA-OVwA-OVwA-G&.A-G&.A-G&.A-F#.A-i##)%Ac^j2ln`:8M,>)4"
        "Rjo;-`E/(%/CRhMPx%V8-,>)4HGl;-V^ps$0I[hM3#&V8Q3G)4]'gN-SPj/%Uku8T[,<i2(EB)4dDp;-qSl)%-V4EG^3EiM%gF(53w&-=mw%V8[1G)4qEoY-/K+.-2QeL2JNoG3h@5dM"
        "3:jI3JUr;-[Y#<-x?S>-#%22%q#eR*kas;-x9uw$bn+x'VT(.?63mk4w06_f2[3.NLY2u7)p]G3EAl;-RK^s$*+EK*iX2u7W%,d3YU8j-)hdR*p#<:2qIo-dFCr;-vRuG-vRuG-#g@Q-"
        "XOsM-.K:@-XOsM-XOsM-@jBK-Pu)M-HD6L-[@)UNS.iH-89OJ-dqrUNh'D*NW4aIN6PsM-_mDE-ox+G-gG8F-b^PLN.$,G-kduk-l3[2V/+90%5nN.N`1N.NG1N.N*2N.NG1N.Np1N.N"
        "H7W.NPrVu72=u`4@@w`4@@w`4BRWA5hlUA5:EMR<RnNR<RnNR<<n[hMEu]/NYkAjMc4]79fG/s7R^1s7R^1s7UpLs7*i1#9Db1(&0(;*N]qJjMblV:8UmL88UmL88GL@)4wwv;-PvTK-"
        "PvTK-62Pp7qTfS8bDp;-'Nl)%9uxDG#eov76VfS8]6s;-'Nl)%?w[hM=2#0N@1#0N@1#0N@1#0N?(^jMD)8r88x/p82oS599SwA-NVwA-NVwA-NVwA-NVwA-NVwA-NVwA-O`<^-`j<F7"
        "OYFF7OYFF7Ggmk4Ggmk4Ggmk4Fadk4_[*3rrNX4:f1G)40#.A-C=n6%/CRhMS=xR9s->)48j9W-NmEF7+4@LMx-<i2w2K)4:VwA-^0,k-OpEF7$W`wK0I[hM*.<i2,oYdtqE=o9wK:&5"
        "su,d333+,Nttc1%1OehM;2X.3:ApfF,HFVC:'=p7uW)=BCfgEH2MN9B9bLQMM6YC5j#**G1/vhFs<(pLQ_?299>>&>9mls-c5?rLZRs0;r2s=BD,/0FMX%e#6S.)<]HnlDPY/oD,SvLF"
        "7T3+$R)Lc$`Sg0F=@p:B%CLlM;$l`<;'bP8q#/d<AudtCuD<;&lGx>H9uR+HVb4SM/L>gLAZ,<-OR+SM43%*NEf(*N,J.q.O5nUC%l=C&VSZt1($'<-eVtGNuQD;:Ho]G3^xl;-iuVu$"
        "W+`ppn0fR9mq]G3*<t;-=%I6%W+`ppjbM:8kq]G3sem;-Tf80%RN^?7n0fR9Dq]G3F=r;-9m<$%UdP<8$o]G3&:n;-sd%3%[YWj;l'SR9SX(gL]'n1M.-pGMQM#<-q4m<-jY#<-kY#<-"
        "lY#<->ld/:T`/g2`/*j$ZJ:&5A^MR<'4Es-YZb.No<a.Nik7f$U60:DlK)gLgIBX9<xZw'xvM'-l'SR95`'gLERrF8`wZw'At:^Zl'SR9A,+gLh-Qu9D`/g2*0^c$sFDF7=x'gL:4j;8"
        "hwZw'9eI,3^Y=99+E51NjEk_$ZGeQ:&/,F%kvXA5ZEXA5IS#kiP#+,N+(Yo$=8'.?Zu(W-VgL'-8=P)NaQBW$(&J_S<vEF7NDxfLB4j;8]`/g2XW6i$Uff2D0=gk$W,iT9</,F%W_x&d"
        "@nC*NH&<.N^%<.N-XBW$X,iT9bOp?7m*SR947wN9$xZw'.S?)4?b7eFl'SR92Q*gL(WJ@9*`/g2PgR^$%:tw9?b$gLa^[I-Xaf[9<k4R*0A$N9Rb/g2#ull$ca;:22M-W-<p@F#8=P)N"
        ")kTW$c0c_/c#).?f?.F[c#).?\?g*_$uf0SDPg[TCfaF,DlmV=B_.9kEfZxJCph)W-<(Tn*wWj-$'FQSD&%q>$P:A_/5P,-Gh>-aEdED9CJ-g20o2>AF'`;pDf'AKNlP^YG07'QqvX,03"
        "mF+7Dju@;Hw',WH%ss#$iH4+$9WjfLwJt]Fj0v<BK#%v8NEl'&nbv;-QncdM9F3NM:E@*NZG1@-Y*dHMO3%*NgsP<-OL+oMDG419D`R$pB,+gLrB>dM%Cb$%^YwQ:p1#c4K[CVC:ln+H"
        "$VnbHxAXVC#$8$H1TpgF%])hFaouS*qk&r2sZ^qD7r-W-708db'M#<-%f`=-=cHj9;p]G3*8q;-A)+$%*</-bo9+o96q]G3.4n;-6A15%Eu,k2uJvWoo9+o9;p]G3OQo;-4M_p$Dk+o9"
        ";p]G3aD_,;gb/g2uKjb$TjA,32B9j9+wZw'cq+x'[+RF%].RF%]+IF%%30W-sv^v7l'SR9[Z%gLUXQhM)h/NMm)m<-s$D*Ni[=g$JX@*5)V;&5)V;&5w=;&51o;&5,a>&5,a>&5,a>&5"
        "gF-R:l'SR9k5&gLDjmN;5wZw'lgq3M@nC*NQw0h$X99.-_s+W-)(JQq8=P)NF]3r$n?ARN;sEF7,u#gLB]ZR<Tc(R<VMU9Tm*SR9mmDE-0*eR:4b/g2pd0W$8<,F[xf4.-%g&gLPx+G-"
        ",a]L<ei4R*8NUXJl'SR98K*gLn0kB-qjmN;Xk4R*D4`G3Fo5X]HH7+NR$1h$HS0x'8t*W-Te$?I8=P)NRoIf$av=_f+M-W--<SqT8=P)N731h$c0c_/5A'gL35-n<7/:kF*WU)<Nb/g2"
        "`o9`$_l&k;k-,F%;D'32O95[$5EMR<sM&gLTDaO;5wZw'ln%a48en;-dGvt$%]#gL2:s'%MN%gLK_+w$rQ)gLIgV,=B`/g2V31h$Znb_/Er'gLtOB%%M4(gL`*4,%UL(gLuBr*%Em^jM"
        "vZ(p$TglW8A.,F%ZTpdbl'SR9N,+gLG7D?=juZw'MUo:'@nC*N)rmX$@T,F[@v'W-I-ZX]t=#c4;sM<BlZ3nD5np%Jk*ZWB(G`iFMRo5Bjiuq8-].f$.?RL2q31F%[+Z,3wOQnE[SUA5"
        "Jp2F%hORF%x1--3@[6WCb*b0M.(29.`lph2]pA,3eMGH3'RMeGIXEkLxO/r/6pkrCoFxUCUlcs-X)xRM[>o/MuaMgCT5=SMu^MgC[hf_/E6O_I>?/9&-ZkVCE8VT.m&4RDH#vs-5Pw3N"
        ",b<'OH/i3COD#iO:J(cHDJqaNrdHiMXh=8MNS4CPgW=CPat2.NCB(@-Aqwr7+ovHHi(bb%gxWb%l49C&l70(&1AW_/1AW_/l:BC&4Js$0i+t'&33SMF4M/799-,F%`h+x'GIi;-%Ou[$"
        "gV#d3s=eDcik.79Ii4R*nmZR*_4RF%f>5dX/G(cH;B5_]<Q(_]>dC_]?d:_]>T(_][Y)_]^f;_]@d:_]^c2_]K*>REa4)W-n[n6qx>Vb=lasJ2>T4.-pNBIbVgZL20J'gLSWk)9EvZw'"
        "F=wJ:[jJ,3v^2m$r@;F70U*iMWkrq8-i4R*9`(g28>'gLs'9Z.KkrLF7xO,/u[QSD77l_&*]cgC$kWR*HrFHM'O(q7ac93CGP@r1c1me3V[If$]Fs$'9W4wp76/$p>X:^#w@aHmrFXN;"
        "XjJ,3e[gk$ZJ:&5e8kJ2J0tV$#[c98s:s=B+[KkCeNg?&eZ/L2dZrrClw1iF$teUCdf@+%Da03M:Qob4S2+gLHAkl(381SD1bUmM>$BeEllE3*7%/TAp5+nDY3I,M$k2OB$Dj9;-p`]A"
        "3N<SMYHr-M*RYUCIWjKOO>b+N8N[oDrv`]A]cDQB->hoDYHdq22:1VC(3f:C=/gGMPc>%PO*s=-ZKo:.&TJ>BGXe;-XEf:.1rw+H&%jJ)XM+BG;%=GHIHUhG,@vgFxg)]AVIo'/I7k=("
        "qjcRE,Vc=(8_LVCm.;C8;pj-?:N'_Im800F=/gGMPc>%PhhXR-_)F(O`Q)=-F,,G-n$9sOv,bbN%w?T-_p*(OX<<4B8A2eGvad[A'v3[T_WD8*ZV-g)UNU/D47lpL>jq@-.lxt?$e.eE"
        "]9XB%MY343)5d++,5im/rrcdGt8fUC1oRq$Oo'99uZ^C->MQ:8oR_k+SB@l+b)B^O11fU.scT0F4,ht7>2)d*1(w51[Pr#8?@`D+%u_ZH#g1eG1J7fGi9XJ-//-]Pfn`wGxVKSD2wW<-"
        "ethq7QAO2(ASt89B>BCO<Ck/OGQ7IMiCEWB3aJLOXHRBOo=re$u%cp'2FB3CQoIr1l;OVC9JJ$OLn$q7>BXmB&jXVCOBcj)(d4'ImKn2Vv)kV$P/5'Iv)sHMgF];%d]cD41',1'(S%[T"
        "@nefG&>.FH.l)a4W@%+%5*'T.c+H.345;q%4+2iF2BR['lSfw'X^ZXLIJWKOTG(cHd?:]09<oP's9XTA4mCq1)DOV(J^`)Fm)0kE=8)=-De@U.4)ddGoo-R8'Gb0PS3+`/`#VgEEn`=-"
        "d@?M&w84</U)P?-s._dN(`a7%/p,+7'5?vPQ#swGxVKSDspj;-lc*B-8guP&2bB&,9H:1Pb2F2,0+3j(uKep7@c,g)8R=%9L_Gr)liT5'?RU5A*jp)53A#/Gkc1eG)9p4&;6S5'CN<)N"
        "l<wQ89fHL,$Fv]m51m<-jfav%];9p&Uc4%'#4-al7:v<-Dbr9.img.*U2igW8Su+H&uha>94)=->1&K'?wEcHMwa0M$OHjB2AvLFE$6h+Un+O092+7D705'I?bf>Qh%6I'[=aD+*n7IM"
        "f/T49VUln*#U%IMjS559rJXMC7e?gL0*Yg$K^v*=52M.3oQm:&%G@bHltY<B&V+WHsTC79ecxfLD/+9'a^49.b;)]&S.@p.4S(*H@wt?0&&t$'$4mKFvij_&DZ(`&XoVU2x4d<'V?<p^"
        "Jjux7k;AbIUPX+lXmZL23=gw'/<IL2?4a,;$=u`4Zq.W-ve@UpVgZL23<*gL(-cW8B<'XU>;'-MQQjN9UkZh,7g`^60+m<-dB0D&H%rM(%;>HMbd*f-o>f;^'M#<-VBm@.jEoo'<]^w'"
        ",#>vHNhR.Q:NO2(*hep7?36XLAC$dMY3%I-6;dtMG/9_O*E2=-Dh2uM2gf-D0j#lE/%oFH%,rEHArQ/):#htqe%G#H:wLqL/W4KMiGo/MI#EI3YK%gLZw)iM2=`H4/?4,HijjjE0mXVC"
        ">:)t-/$@7M&NSF4hrQF%WuQF%f+d;-ClLp$6#'J3rf^G3RiaF5c2G.;g2YD4Z,+gLVO]rL['3iMeRrp$h.%B5wq0oDfx)6BbShdDG*D%':ZTcHd/+UCogeUC<m?#5n:s=BTB&jBmW+b$"
        "pPcdGmJ0nD%DvLF:g(i3&94l(mcPLF1TfUCc<DtB=4ffGR'V1.`*Tr8N>.&GQNxn*4:-0F4ln+Hb'$[AXK*j1ZKBQ/u,41)dTM]ASxk,MPs<FN^-dX9Pe]q)>xfq)PA+gLs>U1Mj:v<-"
        "T3A>-T]Hj9VhgJ)ih7[%i]+KM75nh28[I*H%bV=Bq-i?p8sJF4c(9kExUhS-hH3b$_FQ:AtE>-33&59'ea@uBd;]j1pIcdGsN76D`AhEHZS,iOGEeL21,W<BYuXkDan)'HD9a%8G8+6'"
        "jseUCL,Z<-E[R6/dRv1F:;X4N]^J1Cw,jtB)vxDGVundFT)#o*T9Vo*4piON.d>%Pp4/PDFgF4+#ko[B1O)hF%gcdGNI$U&GCHkE]lR*?bY_n;t)/:C%GI+Hnu[:C6fJRC)S;2F^WS2("
        "1&P&>Q]05NFsweMHwXrLM0.oD8;ZhF=/u;-cW+V.O2JuBs`8V/w?iIGCXg%J;6]5'N%CT&3(CnDX1,SCtg.>B7>.Lu0O;hFYPaQCP?;737-AeG^7V+QSr)WHa]YI3mr(JD>F0@J,EA>B"
        "R7YWB0#DEHqTVZ$9F4,HVwX?-:9)]$2^E.3'mg;-@OJf$M<9E>+;0x?XxZw'/=+:22]j4DYvvh2AFpw'HooA.Z/WI3vvK8/'v(bE@^jKOwdmCFE,f?Kc5R$8<Tp9.'[eBI,8v?g4T'cH"
        "j(`p..$obH()<I$LE%oD25HbNCU;=-d$.78Q`'NC-KkjExsA<-_ZeBOGW[*NjR2=-]g<_O=2;]P`smq&7FcUC;UQbP9e>1%/BK<-%vhf;(I`D+EW2V;DRCO+=a'<->+Z<-;D)=-hVGtQ"
        "5sCm&L(iDl=$BeEn('k*ko8fHCCcn*FN+<-]Z3$PQ3`HMRH`.&CDsLGhA5u/8K.IM/L]D<L#R['@&h<-o;k$PiMrxP:CniMinalE:H7RM@eRIMg=XK(p<1l4E-ABO_973<RocG**%YOk"
        "T-)S*#S=v-&tE.d%GMFQ;GL]&=42eG$]eFHV]-N9o1CWJbG&+4u]DtBW6*[8KbOQ'$teqC9QejD33`HMm0d_Hx1?W-,/xsS9=swG5i3GH1GsQClY>LF>O8(%F@#r)c&qm/*cfYB>l7FH"
        ".F?e%K@'Y(EN-h1?<&NF&PViF?F4,H/a4u9Go&g2gAR,2UUau7l%VR9C7[N):Y7FH`6,*+;u3GH'wAQU+n-NB3hOnB[1$u%>=4,H>+2iFr4-oDfM168H^]q)Y8jDFT/t4NVLPdMEc><-"
        "AUJw7X'QqTj55I;iAGr)3J-<-cGGXQ,vu[PO9(q96Yh[.e7(p$YDs%87DFOO8rm3`F)OFNUkwbNi7V-MIh1*'G4'Y(g&P/).M$i5@c-=SFp&$P3H>wPm9pZPvhaR8,sAa>Nuu[Pq5=J-"
        "l5+cN0u849]0>N)W#x'O/aG@P*1bB-7Gx$(c3]<--vt(OR=98SN)WE-164s7/3/,7*ej<-I*'gNL%5[$3XLKN[BK<(_JT58=F>3D$P9sM/0oI-LE%/&*`;N951(4+pf)a+q3[p75;4K3"
        "L7`=Qaf7Q-kPK%PWVx$Q.*;xPLh#%.Pq;5<^1O2(Is(30w?vsBh/BNBCDuk+isb9.mf,.3Tt$-M@dNBO(KUS28ca-?u*)+48gf@9=L*j1)]v`4ktjD</ZBt8TdF?-ig[,:dtWj1eCMF4"
        "3BH>+U%E)+M%q-MQsEP(JAbgLdmdL2G9k/:vhfq)d-2k`>3,O-AC*r$k84j1,(1Z$rIiC,ZXZIM'w/o8VSw<Cx8eT0Kq%>-of+1)Q%&a+;aZB=oG_k+avel+BH*HMc7D]P;Q,T%*pF*#"
        ";u);8,u?L,G&sj;O'8>-O#hOMZ,LC-DmTp(0wC9BO3J>-Y@R'&%H@qCC)Mq13d&.<UK,L2I*J29,Z-D%MY3433E3m$*a+4iiX2:8$qX;fi?wq$8>R@'swce3+>C,3OR*W-?E]R;[,<.3"
        ">e@68M6D)FW8'r^;$KIEcjJ,3'W9c$;Nwh28+vgF6.OcHF5koM?YQhMJ8%oB2pDe-J'f;-j.oi$`GiS87wgs7>l5kEbD4l4915d2.)lT'&TJ>B02?LFvf^oDc9ht1F_8V8IscL1&NkjE"
        "kWiTCv3?pDtSb-6_uTN<w=5-3cNa,DpZ3nD<<&9&*jXVC@<)dE7hcC&X%+SM'x8.EUkJ,3)Nkb$Qw:_ZxDPg$A+m2;<*TIEOiJ,3%Jr*%@wq/FX#<.3^,+gLr-I]-KaEk`<-geEZjJ,3"
        "0u<Y$9qH2N<-geEl5Fk4FTOs-PXgs?@3`nsBZ*gLKU,u%W]O#H)?OVCmHrMB)j=nEP^*KCD$$&JHvf9&%BOnDaBM=B0Eb)N1/5fG#)(h0'bDtB48vLF9`7rLh&36.?FR1MVZHL2J/-W-"
        "Y:iNaHxOp7a7I6i?e(eMBFK=@$q]G3>e)gLP9=g$G^s)HX#<.3h[a20vpVr&#9BkE4$FeZ:>7%'Gp:qLNnP>'M/B5NA?@^MT[7(>1o]G3q_.W-Norb?I+l58Cl]G3Uj*gLK_+w$s,3r&"
        "Ng[7M#Q.R&`(RvG.2.FHp.oY'kPRhG_-E,37rq;-cbC`$@4*u7]Wb5/n?Hu%=X:qLv-5fGClh;-&#MW$-@Y0lK=i)5$(:oDs'CI847r*4o==rC.Y7FH:sN<BQ,2k9KB*j135)T.t0M*H"
        "HsG<-S6UT&P/5'I4BtN9s.[w'x,=39sJq0,ep2hL3WRSD2T8>B5U6=(k'fA5?PVa%<%DEH4f,uB'$obH@N#<-R:1Q8bh@p&RA<Y$'?vsBm8HFR2v)FRu2dER#G:dM@E%IMER.^$+?t;-"
        "*MKsM(._2BDOkCI`ixZ$=VYc49Ii9.$'4RDs.d8.$tmu%AbX2CR#oRMebFC5kO`t-8-wNNx'3iM*wmgF:K=HMW>5/GKuEw&Wpvh2d;(W-XPIaPjhrq8Si8g2s?u-?Xk*W-+f0<9HxOp7"
        "I<u`4Vg0W-[?rjE5bxW8UZ&g2%8m;-cP`h$iiYD4B4`G3jTs;-1Q+b$QQGW8m,,F%C<U88of(e$G#qjM:P4S90v-A-)=_k$_M?.-Oqw`4D,'gL4v-i$<`;F?Q[_68r:u`48Q&gLa(uo$"
        "9?$.Y/(m<-wxYP2GrpjEPfVL=&>.FHdnViFV``=-5.XY$:JP(5lpY/1xQ?u%2%jMFO2JuBg3xfM;nO(56&h)4O_hMBV2/&G-``A5N&6U8N*>)4<9*gL>hM?%V*?:2I,`qLq#g/NWTC-/"
        "&e?>%_1[.-,T+P(%X;9C/PeFHg(uVCX[?qCGABY(l;)c.&i_.Gm#jP0i`tX(Y^Z:C+)t_&xT#^,3%]h,0u5L>R%:i>K[wY:E]Yk4pvT.-5nS*A2A7xg33OFHp0;9Cm'^e$r[gNEQf#[H"
        "co:+4lE7jB$N'kEeAg;-]Pa]$GbAr8$V@f$D$w;J9_3lCIj4R*(?k;-tO=j$T*i;]<$0iDEiJ,3CH6.%6:H*vQ[_68&(`/WI;w[H*qM*IT5KdDclJ(J'b%gLl9Bs$N8Hs-W)HKNS2^k$"
        "cT7D.jA998:*>)4Fiq;-jg4r$m$TeQ48T<&wi#lEp>FVC*=0W$Wx:TpqE=o9G+>)4^H+W-(E9#08=P)N_<.6%3_3.Noo).NxVC-%Mr2W-8n)Ka^gU0%2WwL2OX2gC@kJ,3>p*$%F,;n)"
        "7C7oBO1G)4:d3$%:@Js-FkE.N^]:k$Ug_kFvGxfL]gU,CR2G)4l48v$QD'gLX3Jn$;5Gs:@nC*N,49n$5EMR<h=1:D@S#gLq^'-%g-%gLg:4qB4->)4m;xfL:GaMHG4Fk4%k`p7]wnPB"
        "tu4<//%Uj24`XnDmHsFG'6FVCluTSDtnVMF?e)[8.QxUC`L/(/ANKw%G)@%HekJ,3bBj'%A^^/Nn[Ts$A^^/N0Z@+%_ZwmUFv$`G%lJ,3Xh)'%sj]WA)peC>o?Y)4Y[a]PI`sn%nAC?7"
        "sP0p846WI-4,p(8>D#E=&>.FH/ZKm_RA?p/-=_oD'i7/GwP5W-IgHTBpU:H4M0;.MYOIb%D`KN2hWNO9m^Th54L9O*b3vsBOQ`MN>n3(Owi`=-vFHeN+K.r7NOvIQ8<vSI^fA,3w;l;-"
        "+'8&%)`h%S]>8+4)ExfLT*)2%=+wt7`,xD?#]@O:=*>)47@)gLiNr2%hp*gL:LOl:4->)4A]#gL6Q;u$oJ4.-QE(.?,6p;-;YZ)%@B/gLKDJv$<eb;Ham4R*ed.vK]A8+4$qF?-WY#<-"
        "`e2EH`Bm926qJ$I,5b@?D+>)4gc1W-Z-ZgjWn).NUsm&%4hNINkk?6%HVIaP&A*/%R4+FPBQcFFQkJ,3iQSq$aPm9ivMCs-W[sU8YL7j4cpaS8e>u`4?G<pIB,.kXc4f9UrNX4:N;AQ*"
        "j_Z.NS8W.NT/S79Xg(^S]A8+4^g^?G<xZw'xM+4NvspL;6+>)4Ju#gLCIR#%c1T0<[5W.3:`(g2-9+g2^u,g22mC+vCZ(#8B/q,-E)[[HbhA,3v$.gLWW,Y;,->)4Mi#gL%PX%8PF:5M"
        "f<4u$sM`CGb.,F%BfqC[')NbM^QhT9a0/W-dHu-?_Y(*4%XvLF?g4HM<@MnBkVv<'tu1eG.`r*H9KGW-L)HGm'M#<-b#<x$Xn2g21o,_$W,iT9xvZw'%9h>&>[cHMG%+,Nqt$d$7b`WJ"
        "t^d--IOER<5]>t&H-`qLU0fWAoc&g2@VYU)Yr;HOum8NMqxDI32Z*gLDNdu%*G%'ItrY1FS^rsBEF#(&D*Ok+iRRF%XxQF%jURF%].RF%]7w;H>Z&hO/%<.Nn*E.N4?cK.@^NI3Q>0Eu"
        "WvDI3(<*gL?vWl(iKU/16Y7fG8#lVC2Pw#JD_.:.*ZX7D&>6I6SR@E5dIcdG;Io6BbD6qi:4[D4hrOHMv#E0CwKvsBeH)]A?OV8&$n/4+]?4kk8HkM(k8wU&=K4m'Zr_8.)`*_I6nV8&"
        "K4xmBh;$@&)`I'IFxeU.w-?LF8a5<-1AjbMtSt&FXg1_SZS#q&eg3nD7_LQM+V'b*FVL<-U=0n*FY9dMw]V`$Lame<d.,F%TlvVHQ&SL2iPp;-jPp;-jMp;-iOD=-5.778/xb597xq0M"
        "lM#<-WY#<-XY#<-1L])<flO59'?8n$R%3.-Xa+W-RYo-F9FlDN$DClLWPwA-.]]Z$mu@6:'.,F%to#d3tx>)4]8YD4^Au`4_J:&5`SUA5hF/s7%hC:8XCJq9e7FVC1G3$JxWvlE.3.%'"
        "DrN<BQU&##%,Guu2+V$#i4Ur]bpd%OiLO2BDA]>,%<1V?4uR4oDQP4olxn3+?X:#vH3dR$Znl-M]HNZPf7$##un0f60.h)N(>.3#1BU/#DwxXuXj:ZuTPPHM,IGB$Dw2d$D6H_[vKRV["
        "lQG;9^A@/;;V>'=#K2^=f*Es3qMaZ?\?OJDO.gIm#lB?h%9fSiB''*h+lWg5#q@HP8kfK#M=C@V8N3uW[wD#N-sJvW[@P:Z-E$tW[,G>2CXeje)Yb<Z[Oi'pA$*Rm#ohNv[?N:Z[5&9]I"
        "#hN9.qBlT8HG+N$K(^m#E^3[9);hMCN[^q)uO9e)M`5>-M_,<.,eF@#Rm#50GaT`<Bd<>]<Z'@0/K)N$.$,j0/k5B?\?*qV[F>q/NhM,>-ot$saNRW5/Y8[B#rcupBew`29Ptc2$OVh@@"
        "l]iE/ANZD-L<%oLI:6^?GOZ55bi_&5R3pr[?2.eQUOXt(N:k>-=3a$'SOv0M6_$u<xc+F-e0WE-1BVC?b2RA-Wt.>-/]5<-qpq&45MPoAJ>/g+VB)W[1&a^[)gQ][tT&=1Ku[w'^r.[["
        "f;-9/Q%(9_of$qL*299_9Bka?Flea?YPvZ?^W:hl#%@2Cq7'=.G#vW[e.H59sH<p._F7'2lAXC?@cf88Aff88X;xiL]kVT15@gKM#6'j0ah0K%lM5/M'nZT-[&,@M[DDj$2-a^[KVxiL"
        "rEBB-0_3uLSV5Z2W2)N$S:&vABONK=Q&0Z?o)TV[<unoLHZFY-&KHOO9g%K%LwDQ8XO`Y#_0>?86UCgL&Ph88mTg5#EmM^[I@'_[9(/Q#f35N06Q(01EqXG/;o99_b`qpLDUS/.KU=5M"
        "a1tv8#2(O1hAJ9](VBP8Ki%[-r'EW-:m$6:7*`dMbkji0c$vhL&xJE-xZa5B#KTV[cRt`-JU#V[N9m`-0Q2nW&//W[%)^5Bp&aE-&44^-Z,g34En'7jh]#Q'u='0/E?;$#S@,XCcs9Z-"
        "JVoM1lG/t[sVLZ[Z%ZQ1=L0kLO.Y>-q.dm:'$VC?JR#<.*=rA(&HSD=8HO2(WZwM1ZIk<::D76Mh),2_cf$qLga;W[MRKE-4@TV[hW5?.?2%11xvmx[)g]^?km[KCG+wW[TIl-=JY<lM"
        "?TFY-E,L/)e3gM)i2if(q#nx[;&;Z[#e4Z[jKY][o%Im#KI_&(0-O2()BvV[(Zt>-BhDe<.A2##&;+N$JbdOOa_hD.LP_;.oYs_&XZ*.-SKD^#2]:#M<5voL,v&nLXJ4tAorfAZx*RV["
        "C/eU%[:)##N`DZ[nih5#lLM>#`0>?8BD@11,L`Y#qmQ][f<9B-Jta9.FnW9.&x;?85H[hL7``pLN+4t*MkFJ(#]'58=;B%B=kI0YIl/t[qw`W[m'cu-qCi;*$Bug)CL.W*07u+M<VV11"
        "b+5F%=e/t[)lu8.uM2N$AV@a+4OC,M@o%21f75F%Aq/t[-lu8.'sIg%E%X#-8hh,MPwX31XKO,8:,&X[)Wj*>BD+j1MMPg+Ti>nEl;]1,SBI1,VC11,m>]1,dsI1,WF11,?8`'#PVcTS"
        "N^?T-#8qDMB-(S-re'S-qe'S-+:XCMED4jLXR#<--:XCMGPFjLXR#<-/:XCMI]XjLXR#<-1:XCMKikjLXR#<-3:XCMb^u311^-Q#;&g>2+UC^[TeiS8PF%U.>=*p.Cb_&4M-%[-^<641"
        "4DbW[Z?tW[l5Y>-MG3mh:/^5B)/HDO9[4h-3bqY?u[.u7Qaxt[j*F<8F:KU[5QMj$,uGB?\?]du-oi^G3$g6QSuTU41bj+X_H7Iq;U,I59uK]t-B+Y98VG>R*gYcn]#fLP/VWW&5u0jc)"
        "AQ'E4.1nlL0mL12x@HP8EktW[w89I*vwZ55lYnV[M0'qLqsWC5XF;##WI+j1;vh5#;#Im#QwdOOMwc?.`dZVBj-Y][<fi;]s?hc)?\?tW[:$h5#.ZwhLAL7p.,f&KM6Jb)'wt=d&,=hKO"
        "`pX>-koDk=2+3D/mC$LM@4RV[l5,KN`-SV[LJXD-i@bC?Uu#Z$l&DN$mI0Y?@5v>-ANAB-2D(tL36)Q9(f)Q8QB0U.U_,Q#83Lh$P(esAa6Dj$'#6k<tXmP8-?D_&]A[][djPS3mu$[T"
        "&RW5/&;7-5Eai?BI1SEnsS7_SAcM>CQ@#B#sg([K>0]3FwNEp.DxvuPq02].Y$)N$qB@I-H:U29q8cRE(#vW[^jG9%SgX>-@ffs7'Q,<.vjtW[&Cx:.Clf88-tuP9'1@nEKoipL3FAv7"
        "i2&beQGZ20+<%D?&+T>.K99q`4VSU)q5wGOY;`/%;]mL-/UIf&q<[58/./pA;2E/%aAxiL]7cGOA+p/Nle&pAfdfpCu/,algMn510i7R95q$S91q5B?:4/`?1jg]?4fNC#@BtW[0Xxu-"
        "nJ3+P/Z_)P9_]cOWAeX.(1<m53<^r1*:W26oQ]>-9Cn8.m)j7T)E`/%v1,Z2X?L59nR5?$#RW5/]J(T.ehN9.VjZi$D-WQ8tf$?6x&W&5@foQ9eEF<$;gh5#Jhx63(8%TgO0Y<8ZcxV["
        "GQF9B;p:?6fDGY-*g&32rsqY?i)[m9;GSY5sORF%q-'@'#Y.[-OSDu$f*[a#p+k61,WVR8T9S9]:?hc))U&X[b?tW[C3:o/L*q5#Q`0U[#ht;.0(wP5oZvf;ebE-*/D+j17uqF.O7(=%"
        ")QlW[CS2Z[$CtW[Q0$6#IOIb%tnIb%%0Pb%wlL9]/:)=-/2#HM*g>oLP_Zi9=G5&>$]bA#$TOA>U)OX($+Jb%6Ph#?X@)##cCCb.':f'&'?u'&*8iT]2I;=-9AGdM4A2pLd=?>#0eF?-"
        "+Z5<-'`5<-PeRfM7SMpL:I;=-;Wob-g[$C/.IJb%%0Pb%<n`6E;:]PBCHNX(CJ4O+en.nLMx.qL%+p+M3(8qLH:)=-C2#HM@F+RMLcamLTL4RMh^5<-,9t;M;XFRMh^5<-.9t;M=eXRM"
        "h^5<-09t;M?qkRMl,MT.q*q5#jb(i%E^:@'cQAmLC7LoMgNEmLE#5;1nJ-.M^WkG8K;X#H>`=;6lQx>-U>p,MaEUSMf6D.M`9(sLEnofN?RhSM[PL@-Kj><-N%kB-Z>p,MoQLsLMRsiM"
        "AtQpMF<]>-^>p,MddhsL5txfN@'RTM$[^C-a>p,Mx;*qMhTNmLL'7tL@X&jMEH<qMF<]>-e>p,Mk8RtL6$,gNGQ<UM$[^C-Yj><-KiNB-i>p,M'QwtLA_/jMLs&rMF<]>-l>p,MMAl`O"
        "c*8$#jGXC?R$Z7]rU>290I/a+9IT>8mN.KCK0Ad<fwIN%ICKfLSB_6_A5pC?f2DYP&Lbc;p9O'GtNg58qqao7%PQ##`9^K1mBVC$Vi=Y%qag:%vb&u(@pkf:BM?g1'IYGM_Q@68r)r_("
        "f$B.&*ej)#aRXgLlKn68Uv3gN?v(.257>##=cDg.VndC?J7VI?=XX2Cpx,0Mk(4h%3eRq.1S.g+<Tbt(:bc/Li+kS7=J+PM]_i=-]JVS%?Via<RX+#5rUtA+fWoN0c3X(NmpL(]sf?W]"
        "=v]##VF%##$,>>######uCTM<FX%##";

    /*
    #version 450 core

    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aUV;
    layout (location = 2) in vec4 aColor;
    layout (push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

    out gl_PerVertex { vec4 gl_Position; };
    layout (location = 0) out struct { vec4 Color; vec2 UV; } Out;

    void main()
    {
        Out.Color = aColor;
        Out.UV = aUV;
        gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
    }
*/

    static std::vector<uint8_t> imguiVertexShader = {
        0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x08, 0x00, 0x2e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x0f, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
        0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
        0x1b, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
        0xc2, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
        0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x06, 0x00, 0x05, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x6f, 0x6c, 0x6f,
        0x72, 0x00, 0x00, 0x00, 0x06, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x55, 0x56, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x4f, 0x75, 0x74, 0x00,
        0x05, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x61, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00,
        0x05, 0x00, 0x03, 0x00, 0x15, 0x00, 0x00, 0x00, 0x61, 0x55, 0x56, 0x00, 0x05, 0x00, 0x06, 0x00,
        0x19, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x50, 0x65, 0x72, 0x56, 0x65, 0x72, 0x74, 0x65, 0x78,
        0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x05, 0x00, 0x03, 0x00,
        0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x00,
        0x61, 0x50, 0x6f, 0x73, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x75, 0x50, 0x75, 0x73, 0x68, 0x43, 0x6f, 0x6e, 0x73, 0x74, 0x61, 0x6e, 0x74, 0x00, 0x00, 0x00,
        0x06, 0x00, 0x05, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x53, 0x63, 0x61,
        0x6c, 0x65, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x75, 0x54, 0x72, 0x61, 0x6e, 0x73, 0x6c, 0x61, 0x74, 0x65, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x70, 0x63, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x15, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x19, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00,
        0x19, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x03, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x16, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00,
        0x08, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x04, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00,
        0x0c, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00,
        0x0c, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
        0x0e, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00,
        0x0c, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x17, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x03, 0x00,
        0x19, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x1a, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x1a, 0x00, 0x00, 0x00,
        0x1b, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x14, 0x00, 0x00, 0x00,
        0x1c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x04, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x1f, 0x00, 0x00, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x1f, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00,
        0x05, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
        0x0f, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x12, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
        0x15, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x17, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x18, 0x00, 0x00, 0x00,
        0x16, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00,
        0x1c, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x23, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x85, 0x00, 0x05, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x24, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00,
        0x21, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00,
        0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00,
        0x81, 0x00, 0x05, 0x00, 0x08, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
        0x26, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00,
        0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x2b, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00,
        0x28, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00, 0x00,
        0x2d, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00,
        0x2d, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00
    };

    /*
        #version 450 core

        layout (location = 0) out vec4 fColor;
        layout (set=0, binding=0) uniform sampler2D sTexture;
        layout (location = 0) in struct { vec4 Color; vec2 UV; } In;

        void main()
        {
            fColor = In.Color * texture(sTexture, In.UV.st);
        }
    */

    static std::vector<uint8_t> imguiFragmentShader = {
        0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x08, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x0f, 0x00, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
        0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
        0xc2, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
        0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x66, 0x43, 0x6f, 0x6c,
        0x6f, 0x72, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x06, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x6f, 0x6c, 0x6f,
        0x72, 0x00, 0x00, 0x00, 0x06, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x55, 0x56, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x49, 0x6e, 0x00, 0x00,
        0x05, 0x00, 0x05, 0x00, 0x16, 0x00, 0x00, 0x00, 0x73, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65,
        0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x16, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x16, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00,
        0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x19, 0x00, 0x09, 0x00, 0x13, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x03, 0x00, 0x14, 0x00, 0x00, 0x00,
        0x13, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x15, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x19, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00,
        0x41, 0x00, 0x05, 0x00, 0x10, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
        0x0f, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00,
        0x11, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x14, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00,
        0x16, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x19, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00,
        0x0d, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00,
        0x1b, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x57, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x1c, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x85, 0x00, 0x05, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00,
        0x3e, 0x00, 0x03, 0x00, 0x09, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00,
        0x38, 0x00, 0x01, 0x00
    };

    ImGuiKey keyToImGui(coffee::Keys key)
    {
        switch (key) {
        case coffee::Keys::Tab:
            return ImGuiKey_Tab;
        case coffee::Keys::Left:
            return ImGuiKey_LeftArrow;
        case coffee::Keys::Right:
            return ImGuiKey_RightArrow;
        case coffee::Keys::Up:
            return ImGuiKey_UpArrow;
        case coffee::Keys::Down:
            return ImGuiKey_DownArrow;
        case coffee::Keys::PageUp:
            return ImGuiKey_PageUp;
        case coffee::Keys::PageDown:
            return ImGuiKey_PageDown;
        case coffee::Keys::Home:
            return ImGuiKey_Home;
        case coffee::Keys::End:
            return ImGuiKey_End;
        case coffee::Keys::Insert:
            return ImGuiKey_Insert;
        case coffee::Keys::Delete:
            return ImGuiKey_Delete;
        case coffee::Keys::Backspace:
            return ImGuiKey_Backspace;
        case coffee::Keys::Space:
            return ImGuiKey_Space;
        case coffee::Keys::Enter:
            return ImGuiKey_Enter;
        case coffee::Keys::Escape:
            return ImGuiKey_Escape;
        case coffee::Keys::Apostrophe:
            return ImGuiKey_Apostrophe;
        case coffee::Keys::Comma:
            return ImGuiKey_Comma;
        case coffee::Keys::Minus:
            return ImGuiKey_Minus;
        case coffee::Keys::Period:
            return ImGuiKey_Period;
        case coffee::Keys::Slash:
            return ImGuiKey_Slash;
        case coffee::Keys::Semicolon:
            return ImGuiKey_Semicolon;
        case coffee::Keys::Equal:
            return ImGuiKey_Equal;
        case coffee::Keys::LeftBracket:
            return ImGuiKey_LeftBracket;
        case coffee::Keys::Backslash:
            return ImGuiKey_Backslash;
        case coffee::Keys::RightBracket:
            return ImGuiKey_RightBracket;
        case coffee::Keys::GraveAccent:
            return ImGuiKey_GraveAccent;
        case coffee::Keys::CapsLock:
            return ImGuiKey_CapsLock;
        case coffee::Keys::ScrollLock:
            return ImGuiKey_ScrollLock;
        case coffee::Keys::NumLock:
            return ImGuiKey_NumLock;
        case coffee::Keys::PrintScreen:
            return ImGuiKey_PrintScreen;
        case coffee::Keys::Pause:
            return ImGuiKey_Pause;
        case coffee::Keys::KP0:
            return ImGuiKey_Keypad0;
        case coffee::Keys::KP1:
            return ImGuiKey_Keypad1;
        case coffee::Keys::KP2:
            return ImGuiKey_Keypad2;
        case coffee::Keys::KP3:
            return ImGuiKey_Keypad3;
        case coffee::Keys::KP4:
            return ImGuiKey_Keypad4;
        case coffee::Keys::KP5:
            return ImGuiKey_Keypad5;
        case coffee::Keys::KP6:
            return ImGuiKey_Keypad6;
        case coffee::Keys::KP7:
            return ImGuiKey_Keypad7;
        case coffee::Keys::KP8:
            return ImGuiKey_Keypad8;
        case coffee::Keys::KP9:
            return ImGuiKey_Keypad9;
        case coffee::Keys::KPDecimal:
            return ImGuiKey_KeypadDecimal;
        case coffee::Keys::KPDivide:
            return ImGuiKey_KeypadDivide;
        case coffee::Keys::KPMultiply:
            return ImGuiKey_KeypadMultiply;
        case coffee::Keys::KPSubtract:
            return ImGuiKey_KeypadSubtract;
        case coffee::Keys::KPAdd:
            return ImGuiKey_KeypadAdd;
        case coffee::Keys::KPEnter:
            return ImGuiKey_KeypadEnter;
        case coffee::Keys::KPEqual:
            return ImGuiKey_KeypadEqual;
        case coffee::Keys::LeftShift:
            return ImGuiKey_LeftShift;
        case coffee::Keys::LeftControl:
            return ImGuiKey_LeftCtrl;
        case coffee::Keys::LeftAlt:
            return ImGuiKey_LeftAlt;
        case coffee::Keys::LeftSuper:
            return ImGuiKey_LeftSuper;
        case coffee::Keys::RightShift:
            return ImGuiKey_RightShift;
        case coffee::Keys::RightControl:
            return ImGuiKey_RightCtrl;
        case coffee::Keys::RightAlt:
            return ImGuiKey_RightAlt;
        case coffee::Keys::RightSuper:
            return ImGuiKey_RightSuper;
        case coffee::Keys::Menu:
            return ImGuiKey_Menu;
        case coffee::Keys::D0:
            return ImGuiKey_0;
        case coffee::Keys::D1:
            return ImGuiKey_1;
        case coffee::Keys::D2:
            return ImGuiKey_2;
        case coffee::Keys::D3:
            return ImGuiKey_3;
        case coffee::Keys::D4:
            return ImGuiKey_4;
        case coffee::Keys::D5:
            return ImGuiKey_5;
        case coffee::Keys::D6:
            return ImGuiKey_6;
        case coffee::Keys::D7:
            return ImGuiKey_7;
        case coffee::Keys::D8:
            return ImGuiKey_8;
        case coffee::Keys::D9:
            return ImGuiKey_9;
        case coffee::Keys::A:
            return ImGuiKey_A;
        case coffee::Keys::B:
            return ImGuiKey_B;
        case coffee::Keys::C:
            return ImGuiKey_C;
        case coffee::Keys::D:
            return ImGuiKey_D;
        case coffee::Keys::E:
            return ImGuiKey_E;
        case coffee::Keys::F:
            return ImGuiKey_F;
        case coffee::Keys::G:
            return ImGuiKey_G;
        case coffee::Keys::H:
            return ImGuiKey_H;
        case coffee::Keys::I:
            return ImGuiKey_I;
        case coffee::Keys::J:
            return ImGuiKey_J;
        case coffee::Keys::K:
            return ImGuiKey_K;
        case coffee::Keys::L:
            return ImGuiKey_L;
        case coffee::Keys::M:
            return ImGuiKey_M;
        case coffee::Keys::N:
            return ImGuiKey_N;
        case coffee::Keys::O:
            return ImGuiKey_O;
        case coffee::Keys::P:
            return ImGuiKey_P;
        case coffee::Keys::Q:
            return ImGuiKey_Q;
        case coffee::Keys::R:
            return ImGuiKey_R;
        case coffee::Keys::S:
            return ImGuiKey_S;
        case coffee::Keys::T:
            return ImGuiKey_T;
        case coffee::Keys::U:
            return ImGuiKey_U;
        case coffee::Keys::V:
            return ImGuiKey_V;
        case coffee::Keys::W:
            return ImGuiKey_W;
        case coffee::Keys::X:
            return ImGuiKey_X;
        case coffee::Keys::Y:
            return ImGuiKey_Y;
        case coffee::Keys::Z:
            return ImGuiKey_Z;
        case coffee::Keys::F1:
            return ImGuiKey_F1;
        case coffee::Keys::F2:
            return ImGuiKey_F2;
        case coffee::Keys::F3:
            return ImGuiKey_F3;
        case coffee::Keys::F4:
            return ImGuiKey_F4;
        case coffee::Keys::F5:
            return ImGuiKey_F5;
        case coffee::Keys::F6:
            return ImGuiKey_F6;
        case coffee::Keys::F7:
            return ImGuiKey_F7;
        case coffee::Keys::F8:
            return ImGuiKey_F8;
        case coffee::Keys::F9:
            return ImGuiKey_F9;
        case coffee::Keys::F10:
            return ImGuiKey_F10;
        case coffee::Keys::F11:
            return ImGuiKey_F11;
        case coffee::Keys::F12:
            return ImGuiKey_F12;
        default:
            return ImGuiKey_None;
        }
    }

    ImGuiImplementation::ImGuiImplementation(const coffee::graphics::DevicePtr& device, const coffee::graphics::WindowPtr& applicationWindow)
        : device { device }
        , applicationWindow { applicationWindow }
    {
        initializeImGui();
        initializeBackend();
    }

    ImGuiImplementation::~ImGuiImplementation()
    {
        vkDeviceWaitIdle(device->logicalDevice());

        ImGui::DestroyPlatformWindows();

        delete static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        delete static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        ImGui::DestroyContext();
    }

    void ImGuiImplementation::update(float deltaTime)
    {
        ImGuiIO& io = ImGui::GetIO();

        VkExtent2D windowSize = applicationWindow->windowSize();
        io.DisplaySize = ImVec2 { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) };

        if (windowSize.width > 0 && windowSize.height > 0) {
            VkExtent2D framebufferSize = applicationWindow->framebufferSize();

            io.DisplayFramebufferScale = ImVec2 { static_cast<float>(windowSize.width / framebufferSize.width),
                                                  static_cast<float>(windowSize.height / framebufferSize.height) };
        }

        io.DeltaTime = deltaTime;

        if (static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors) {
            updateMonitors();
        }

        updateMouse(applicationWindow);
        updateCursor(applicationWindow);

        acquired = applicationWindow->acquireNextImage();
    }

    void ImGuiImplementation::render()
    {
        ImGuiIO& io = ImGui::GetIO();

        if (acquired) {
            coffee::graphics::CommandBuffer commandBuffer = coffee::graphics::CommandBuffer::createGraphics(device);
            this->render(ImGui::GetMainViewport(), commandBuffer);
            applicationWindow->sendCommandBuffer(std::move(commandBuffer));
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    bool ImGuiImplementation::isAnyWindowActive()
    {
        for (ImGuiViewportP* viewport : ImGui::GetCurrentContext()->Viewports) {
            if (ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)) {
                if (!viewportData->windowHandle->isIconified()) {
                    return true;
                }
            }
        }

        return false;
    }

    void ImGuiImplementation::render(ImGuiViewport* viewport, const coffee::graphics::CommandBuffer& commandBuffer)
    {
        ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        ImGuiRendererData* rendererData = static_cast<ImGuiRendererData*>(viewport->RendererUserData);
        ImDrawData* data = viewport->DrawData;

        const uint32_t frameIndex = viewportData->windowHandle->currentImageIndex();
        float framebufferWidth = data->DisplaySize.x * data->FramebufferScale.x;
        float framebufferHeight = data->DisplaySize.y * data->FramebufferScale.y;

        if (framebufferWidth <= 0.0f || framebufferHeight <= 0.0f) {
            return;
        }

        commandBuffer.beginRenderPass(backendData->renderPass, rendererData->framebuffers[frameIndex], {
            .offset = { 0, 0 },
            .extent = viewportData->windowHandle->framebufferSize()
            }, static_cast<uint32_t>(clearValues.size()), clearValues.data());

        if (data->TotalVtxCount > 0) {
            size_t vertexSize = data->TotalVtxCount * sizeof(ImDrawVert);
            size_t indexSize = data->TotalIdxCount * sizeof(ImDrawIdx);

            coffee::graphics::BufferPtr& vertexBuffer = rendererData->vertexBuffers[frameIndex];
            coffee::graphics::BufferPtr& indexBuffer = rendererData->indexBuffers[frameIndex];

            if (vertexBuffer == nullptr || (vertexBuffer->instanceSize * vertexBuffer->instanceCount) < vertexSize) {
                if (vertexBuffer != nullptr) {
                    vertexBuffer->unmap();
                }

                coffee::graphics::BufferConfiguration configuration{};
                configuration.instanceSize = sizeof(ImDrawVert);
                configuration.instanceCount = data->TotalVtxCount;
                configuration.usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                configuration.allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

                vertexBuffer = coffee::graphics::Buffer::create(backendData->device, configuration);
                vertexBuffer->map();
            }

            if (indexBuffer == nullptr || (indexBuffer->instanceSize * indexBuffer->instanceCount) < indexSize) {
                if (indexBuffer != nullptr) {
                    indexBuffer->unmap();
                }

                coffee::graphics::BufferConfiguration configuration{};
                configuration.instanceSize = sizeof(ImDrawIdx);
                configuration.instanceCount = data->TotalIdxCount;
                configuration.usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                configuration.allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

                indexBuffer = coffee::graphics::Buffer::create(backendData->device, configuration);
                indexBuffer->map();
            }

            size_t vertexOffset = 0;
            size_t indexOffset = 0;

            for (size_t i = 0; i < data->CmdListsCount; i++) {
                const ImDrawList* list = data->CmdLists[i];

                std::memcpy(static_cast<char*>(vertexBuffer->memory()) + vertexOffset,
                    list->VtxBuffer.Data, list->VtxBuffer.Size * sizeof(ImDrawVert));
                std::memcpy(static_cast<char*>(indexBuffer->memory()) + indexOffset,
                    list->IdxBuffer.Data, list->IdxBuffer.Size * sizeof(ImDrawIdx));

                vertexOffset += list->VtxBuffer.Size * sizeof(ImDrawVert);
                indexOffset += list->IdxBuffer.Size * sizeof(ImDrawIdx);
            }

            VkDeviceSize offsets[] = { 0 };
            commandBuffer.bindVertexBuffers(1, &vertexBuffer->buffer(), offsets);
            commandBuffer.bindIndexBuffer(indexBuffer);
        }

        commandBuffer.bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, backendData->pipeline);
        commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, backendData->pipeline, backendData->descriptorSet);
        commandBuffer.setViewport({ .width = framebufferWidth, .height = framebufferHeight, .minDepth = 0.0f, .maxDepth = 1.0f });

        ImGuiPushConstant constants{};
        constants.scale.x = 2.0f / data->DisplaySize.x;
        constants.scale.y = 2.0f / data->DisplaySize.y;
        constants.translate.x = -1.0f - data->DisplayPos.x * constants.scale.x;
        constants.translate.y = -1.0f - data->DisplayPos.y * constants.scale.y;
        commandBuffer.pushConstants(backendData->pipeline, VK_SHADER_STAGE_VERTEX_BIT, sizeof(ImGuiPushConstant), &constants);

        ImVec2 clipOff = data->DisplayPos;
        ImVec2 clipScale = data->FramebufferScale;

        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;

        for (int i = 0; i < data->CmdListsCount; i++) {
            const ImDrawList* list = data->CmdLists[i];

            for (int j = 0; j < list->CmdBuffer.Size; j++) {
                const ImDrawCmd& command = list->CmdBuffer[j];

                if (command.UserCallback != nullptr) {
                    command.UserCallback(list, &command);
                    continue;
                }

                ImVec2 clipMin { (command.ClipRect.x - clipOff.x) * clipScale.x, (command.ClipRect.y - clipOff.y) * clipScale.y };
                ImVec2 clipMax { (command.ClipRect.z - clipOff.x) * clipScale.x, (command.ClipRect.w - clipOff.y) * clipScale.y };

                clipMin.x = std::max(clipMin.x, 0.0f);
                clipMin.y = std::max(clipMin.y, 0.0f);

                clipMax.x = std::min(clipMax.x, framebufferWidth);
                clipMax.y = std::min(clipMax.y, framebufferHeight);

                if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y) {
                    continue;
                }

                if (command.TextureId != nullptr) {
                    commandBuffer.bindDescriptorSets(
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        backendData->pipeline,
                        *reinterpret_cast<const coffee::graphics::DescriptorSetPtr*>(command.TextureId));
                }

                commandBuffer.setScissor({
                    .offset = {
                        .x = static_cast<int32_t>(clipMin.x),
                        .y = static_cast<int32_t>(clipMin.y)
                    },
                    .extent = {
                        .width = static_cast<uint32_t>(clipMax.x - clipMin.x),
                        .height = static_cast<uint32_t>(clipMax.y - clipMin.y)
                    }
                });
                commandBuffer.drawIndexed(command.ElemCount, 1U, indexOffset + command.IdxOffset, vertexOffset + command.VtxOffset);

                // We must restore default descriptor set
                if (command.TextureId != nullptr) {
                    commandBuffer.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, backendData->pipeline, backendData->descriptorSet);
                }
            }

            vertexOffset += list->VtxBuffer.Size;
            indexOffset += list->IdxBuffer.Size;
        }

        commandBuffer.endRenderPass();
    }

    void ImGuiImplementation::initializeImGui()
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
        io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;

        VkExtent2D windowSize = applicationWindow->windowSize();
        io.DisplaySize = ImVec2 { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) };

        ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(io.BackendPlatformUserData = new ImGuiBackendPlatformData {});

        backendData->cursors[ImGuiMouseCursor_Arrow] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::Arrow);
        backendData->cursors[ImGuiMouseCursor_TextInput] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::TextInput);
        backendData->cursors[ImGuiMouseCursor_Hand] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::Hand);
        backendData->cursors[ImGuiMouseCursor_ResizeEW] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::ResizeEW);
        backendData->cursors[ImGuiMouseCursor_ResizeNS] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::ResizeNS);
        backendData->cursors[ImGuiMouseCursor_ResizeNWSE] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::ResizeNWSE);
        backendData->cursors[ImGuiMouseCursor_ResizeNESW] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::ResizeNESW);
        backendData->cursors[ImGuiMouseCursor_ResizeAll] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::ResizeAll);
        backendData->cursors[ImGuiMouseCursor_NotAllowed] = coffee::graphics::Cursor::create(coffee::graphics::CursorType::NotAllowed);

        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        updateMonitors();

        coffee::graphics::Monitor::monitorConnectedEvent += [](const coffee::graphics::MonitorPtr&) {
            static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors = true;
        };

        coffee::graphics::Monitor::monitorDisconnectedEvent += [](const coffee::graphics::MonitorPtr&) {
            static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors = true;
        };

        // Platform callbacks
        platformIO.Platform_CreateWindow = [](ImGuiViewport* viewport) {
            ImGuiBackendPlatformData* backendPlatformData = static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
            ImGuiBackendRendererData* backendRendererData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

            ImGuiViewportData* viewportData = new ImGuiViewportData{
                .windowHandle = coffee::graphics::Window::create(backendRendererData->device, {
                    .extent = {.width = static_cast<uint32_t>(viewport->Size.x), .height = static_cast<uint32_t>(viewport->Size.y) },
                    .presentMode = presentMode,
                    .hiddenOnStart = true
                    // BUG: No borderless here because if window is collapsed it will crash engine with 0 height size
                }).release()  // We must explicitly delete this
            };

            viewport->PlatformUserData = viewportData;

            coffee::graphics::Window* window = viewportData->windowHandle;

            window->userData = viewport->ID;
            window->setWindowPosition({ static_cast<int32_t>(viewport->Pos.x), static_cast<int32_t>(viewport->Pos.y) });

            window->windowFocusEvent += focusCallback;
            window->windowEnterEvent += enterCallback;
            window->mouseClickEvent += mouseClickCallback;
            window->mouseMoveEvent += mousePositionCallback;
            window->mouseWheelEvent += mouseWheelCallback;
            window->keyEvent += keyCallback;
            window->charEvent += charCallback;

            window->windowPositionEvent += [](const coffee::graphics::Window& window, const coffee::WindowPositionEvent& e) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window.userData))) {
                    if (ImGui::GetFrameCount() <= static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->ignoreWindowPositionFrameCount + 1) {
                        return;
                    }

                    viewport->PlatformRequestMove = true;
                }
            };

            window->windowResizeEvent += [](const coffee::graphics::Window& window, const coffee::ResizeEvent& e) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window.userData))) {
                    ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
                    ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
                    ImGuiRendererData* rendererData = static_cast<ImGuiRendererData*>(viewport->RendererUserData);

                    const auto& presentImages = viewportData->windowHandle->presentImages();
                    rendererData->swapChainViews.resize(presentImages.size());
                    rendererData->framebuffers.resize(presentImages.size());

                    for (size_t i = 0; i < presentImages.size(); i++) {
                        rendererData->swapChainViews[i] = coffee::graphics::ImageView::create(presentImages[i], {
                            .viewType = VK_IMAGE_VIEW_TYPE_2D,
                            .format = backendData->device->surfaceFormat()
                            });

                        rendererData->framebuffers[i] = coffee::graphics::Framebuffer::create(backendData->device, backendData->renderPass, {
                            .extent = viewportData->windowHandle->framebufferSize(),
                            .colorViews = { rendererData->swapChainViews[i] }
                            });
                    }

                    if (ImGui::GetFrameCount() <= static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->ignoreWindowSizeFrameCount + 1) {
                        return;
                    }

                    viewport->PlatformRequestResize = true;
                }
            };

            window->windowCloseEvent += [](const coffee::graphics::Window& window) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window.userData))) {
                    viewport->PlatformRequestClose = true;
                }
            };

            if ((viewport->Flags & ImGuiViewportFlags_NoDecoration) != 0) {
                window->makeBorderless();
            }

            window->showWindow();
        };

        platformIO.Platform_DestroyWindow = [](ImGuiViewport* viewport) {
            if (ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)) {
                ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
                ImGuiRendererData* rendererData = static_cast<ImGuiRendererData*>(viewport->RendererUserData);

                for (int32_t i = 0; i < IM_ARRAYSIZE(backendData->keyOwnerWindows); i++) {
                    if (backendData->keyOwnerWindows[i] == viewportData->windowHandle) {
                        keyCallback(*viewportData->windowHandle, { coffee::State::Release, static_cast<coffee::Keys>(i), 0, 0 });
                    }
                }

                for (const auto& buffer : rendererData->vertexBuffers) {
                    if (buffer != nullptr) {
                        buffer->unmap();
                    }
                }

                for (const auto& buffer : rendererData->indexBuffers) {
                    if (buffer != nullptr) {
                        buffer->unmap();
                    }
                }

                // If user destroys window while cursor is captured
                if (viewportData->windowHandle == backendData->fullControlWindowPtr) {
                    backendData->fullControlWindowPtr = nullptr;
                }

                // We released this handle before, so we must explicitly delete it
                if (viewport != ImGui::GetMainViewport()) {
                    delete viewportData->windowHandle;
                }

                delete viewportData;
                viewport->PlatformUserData = nullptr;

                delete rendererData;
                viewport->RendererUserData = nullptr;
            }
        };

        platformIO.Platform_ShowWindow = [](ImGuiViewport* viewport) {
            static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle->showWindow();
        };

        platformIO.Platform_SetWindowPos = [](ImGuiViewport* viewport, ImVec2 position) {
            ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
            viewportData->ignoreWindowPositionFrameCount = ImGui::GetFrameCount();
            viewportData->windowHandle->setWindowPosition({ static_cast<int32_t>(position.x), static_cast<int32_t>(position.y) });
        };

        platformIO.Platform_GetWindowPos = [](ImGuiViewport* viewport) -> ImVec2 {
            coffee::graphics::Window* window = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;
            VkOffset2D windowPosition = window->windowPosition();
            return { static_cast<float>(windowPosition.x), static_cast<float>(windowPosition.y) };
        };

        platformIO.Platform_SetWindowSize = [](ImGuiViewport* viewport, ImVec2 size) {
            ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
            viewportData->ignoreWindowSizeFrameCount = ImGui::GetFrameCount();
            viewportData->windowHandle->setWindowSize({ static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y) });
        };

        platformIO.Platform_GetWindowSize = [](ImGuiViewport* viewport) -> ImVec2 {
            coffee::graphics::Window* window = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;
            VkExtent2D windowSize = window->windowSize();
            return { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) };
        };

        platformIO.Platform_SetWindowFocus = [](ImGuiViewport* viewport) {
            static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle->focusWindow();
        };

        platformIO.Platform_GetWindowFocus = [](ImGuiViewport* viewport) -> bool {
            return static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle->isFocused();
        };

        platformIO.Platform_GetWindowMinimized = [](ImGuiViewport* viewport) -> bool {
            bool result = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle->isIconified();
            return result;
        };

        platformIO.Platform_SetWindowTitle = [](ImGuiViewport* viewport, const char* str) {
            static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle->setWindowTitle(str);
        };

        platformIO.Platform_RenderWindow = [](ImGuiViewport*, void*) { /* Already handled by Renderer_RenderWindow */ };
        platformIO.Platform_SwapBuffers = [](ImGuiViewport*, void*) { /* Already handled by Renderer_SwapBuffers */ };

        // Renderer callbacks
        platformIO.Renderer_CreateWindow = [](ImGuiViewport* viewport) {
            ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
            ImGuiRendererData* rendererData = static_cast<ImGuiRendererData*>(viewport->RendererUserData = new ImGuiRendererData {});

            coffee::graphics::Window* window = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;
            const auto& presentImages = window->presentImages();
            rendererData->swapChainViews.resize(presentImages.size());
            rendererData->framebuffers.resize(presentImages.size());

            for (size_t i = 0; i < presentImages.size(); i++) {
                rendererData->swapChainViews[i] = coffee::graphics::ImageView::create(presentImages[i], {
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = backendData->device->surfaceFormat()
                    });

                rendererData->framebuffers[i] = coffee::graphics::Framebuffer::create(backendData->device, backendData->renderPass, {
                    .extent = window->framebufferSize(),
                    .colorViews = { rendererData->swapChainViews[i] }
                    });
            }

            rendererData->vertexBuffers.resize(presentImages.size());
            rendererData->indexBuffers.resize(presentImages.size());
        };

        platformIO.Renderer_DestroyWindow = [](ImGuiViewport*) {
            // Already handled by Platform_DestroyWindow
            // Must be here as placeholder, otherwise viewports won't be able to merge
        };

        platformIO.Renderer_RenderWindow = [](ImGuiViewport* viewport, void*) {
            ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
            ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);

            if (viewportData->windowHandle->acquireNextImage()) {
                coffee::graphics::CommandBuffer commandBuffer = coffee::graphics::CommandBuffer::createGraphics(backendData->device);
                ImGuiImplementation::render(viewport, commandBuffer);
                viewportData->windowHandle->sendCommandBuffer(std::move(commandBuffer));
            }
        };

        applicationWindow->windowFocusEvent += focusCallback;
        applicationWindow->windowEnterEvent += enterCallback;
        applicationWindow->mouseClickEvent += mouseClickCallback;
        applicationWindow->mouseMoveEvent += mousePositionCallback;
        applicationWindow->mouseWheelEvent += mouseWheelCallback;
        applicationWindow->keyEvent += keyCallback;
        applicationWindow->charEvent += charCallback;

        applicationWindow->windowResizeEvent += [](const coffee::graphics::Window& window, const coffee::ResizeEvent& e) {
            ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
            ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(ImGui::GetMainViewport()->PlatformUserData);
            ImGuiRendererData* rendererData = static_cast<ImGuiRendererData*>(ImGui::GetMainViewport()->RendererUserData);

            const auto& presentImages = viewportData->windowHandle->presentImages();
            rendererData->swapChainViews.resize(presentImages.size());
            rendererData->framebuffers.resize(presentImages.size());

            for (size_t i = 0; i < presentImages.size(); i++) {
                rendererData->swapChainViews[i] = coffee::graphics::ImageView::create(presentImages[i], {
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = backendData->device->surfaceFormat()
                });

                rendererData->framebuffers[i] = coffee::graphics::Framebuffer::create(backendData->device, backendData->renderPass, {
                    .extent = viewportData->windowHandle->framebufferSize(),
                    .colorViews = { rendererData->swapChainViews[i] }
                });
            }
        };

        applicationWindow->userData = 0;

        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        mainViewport->PlatformUserData = new ImGuiViewportData{ .windowHandle = applicationWindow.get() };
    }

    void ImGuiImplementation::initializeBackend()
    {
        ImGuiBackendRendererData* backendData =
            static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData = new ImGuiBackendRendererData {});
        ImGuiRendererData* rendererData =
            static_cast<ImGuiRendererData*>(ImGui::GetMainViewport()->RendererUserData = new ImGuiRendererData {});

        backendData->device = device;

        createFonts();
        createDescriptors();
        createRenderPass();
        createPipeline();

        const auto& presentImages = applicationWindow->presentImages();
        rendererData->swapChainViews.resize(presentImages.size());
        rendererData->framebuffers.resize(presentImages.size());

        for (size_t i = 0; i < presentImages.size(); i++) {
            rendererData->swapChainViews[i] = coffee::graphics::ImageView::create(presentImages[i], {
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = backendData->device->surfaceFormat()
            });

            rendererData->framebuffers[i] = coffee::graphics::Framebuffer::create(backendData->device, backendData->renderPass, {
                .extent = applicationWindow->framebufferSize(),
                .colorViews = { rendererData->swapChainViews[i] }
            });
        }

        rendererData->vertexBuffers.resize(presentImages.size());
        rendererData->indexBuffers.resize(presentImages.size());
    }

    void ImGuiImplementation::createFonts()
    {
        ImGuiIO& io = ImGui::GetIO();
        uint8_t* fontPixels = nullptr;
        int width{}, height{};

        ImFontConfig fontConfiguration{};
        fontConfiguration.SizePixels = 16.0f;
        fontConfiguration.OversampleH = 1;
        fontConfiguration.OversampleV = 1;
        fontConfiguration.PixelSnapH = true;
        fontConfiguration.GlyphOffset.y = std::floor(fontConfiguration.SizePixels / 13.0f);

        io.Fonts->AddFontFromMemoryCompressedBase85TTF(
            FiraFontInBase85,
            fontConfiguration.SizePixels,
            &fontConfiguration,
            io.Fonts->GetGlyphRangesCyrillic()
        );

        io.Fonts->Build();
        io.Fonts->GetTexDataAsAlpha8(&fontPixels, &width, &height);

        ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(io.BackendRendererUserData);

        backendData->fontsSampler = coffee::graphics::Sampler::create(backendData->device, {
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .anisotropyEnable = true,
            .maxAnisotropy = 1U,
            .minLod = -1000.0f,
            .maxLod = 1000.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK
            });

        backendData->fonts = coffee::graphics::Image::create(backendData->device, {
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R8_UNORM,
            .extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .priority = 1.0f
            });

        coffee::graphics::BufferPtr stagingBuffer = coffee::graphics::Buffer::create(backendData->device, {
            .instanceSize = static_cast<uint32_t>(width * height),
            .instanceCount = 1U,
            .usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            .allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
            });

        stagingBuffer->map();
        {
            std::memcpy(stagingBuffer->memory(), fontPixels, static_cast<size_t>(width * height));
            stagingBuffer->flush();
        }
        stagingBuffer->unmap();

        coffee::graphics::SingleTime::copyBufferToImage(device, backendData->fonts, stagingBuffer);

        backendData->fontsView = coffee::graphics::ImageView::create(backendData->fonts, {
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_R8_UNORM,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_ONE,
                .g = VK_COMPONENT_SWIZZLE_ONE,
                .b = VK_COMPONENT_SWIZZLE_ONE,
                .a = VK_COMPONENT_SWIZZLE_R
            }
        });
    }

    void ImGuiImplementation::createDescriptors()
    {
        ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        const std::map<uint32_t, coffee::graphics::DescriptorBindingInfo> bindings{
            { 0, {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .shaderStages = VK_SHADER_STAGE_FRAGMENT_BIT } }
        };

        backendData->layout = coffee::graphics::DescriptorLayout::create(device, bindings);
        backendData->descriptorSet = coffee::graphics::DescriptorSet::create(device, coffee::graphics::DescriptorWriter(backendData->layout)
            .addImage(0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, backendData->fontsView, backendData->fontsSampler));
    }

    void ImGuiImplementation::createRenderPass()
    {
        ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        backendData->renderPass = coffee::graphics::RenderPass::create(device, {
            .colorAttachments = { coffee::graphics::AttachmentConfiguration {
                .format = device->surfaceFormat(),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            }}
        });
    }

    void ImGuiImplementation::createPipeline()
    {
        ImGuiBackendRendererData* backendData = static_cast<ImGuiBackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        backendData->pipeline = coffee::graphics::Pipeline::create(device, backendData->renderPass, {
            .shaders = coffee::utils::moveList<coffee::graphics::ShaderPtr, std::vector<coffee::graphics::ShaderPtr>>({
                coffee::graphics::ShaderModule::create(device, imguiVertexShader, VK_SHADER_STAGE_VERTEX_BIT),
                coffee::graphics::ShaderModule::create(device, imguiFragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT)
            }),
            .layouts = {
                backendData->layout
            },
            .inputBindings = { coffee::graphics::InputBinding {
                .binding = 0U,
                .stride = sizeof(ImDrawVert),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .elements = {
                    coffee::graphics::InputElement {.location = 0U, .format = VK_FORMAT_R32G32_SFLOAT, .offset = offsetof(ImDrawVert, pos) },
                    coffee::graphics::InputElement {.location = 1U, .format = VK_FORMAT_R32G32_SFLOAT, .offset = offsetof(ImDrawVert, uv) },
                    coffee::graphics::InputElement {.location = 2U, .format = VK_FORMAT_R8G8B8A8_UNORM, .offset = offsetof(ImDrawVert, col) }
                }
            }},
            .constantRanges = { coffee::graphics::PushConstantRange {
                .stages = VK_SHADER_STAGE_VERTEX_BIT,
                .size = static_cast<uint32_t>(sizeof(ImGuiPushConstant))
            }},
            .rasterizationInfo = {
                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE
            },
            .colorBlendAttachment = {
                .blendEnable = true,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
            },
            .depthStencilInfo = {
                .depthTestEnable = false,
                .depthWriteEnable = false,
                .depthCompareOp = VK_COMPARE_OP_NEVER
            }
        });
    }

    void ImGuiImplementation::focusCallback(const coffee::graphics::Window& window, const coffee::WindowFocusEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(!e.lost);
    }

    void ImGuiImplementation::enterCallback(const coffee::graphics::Window& window, const coffee::WindowEnterEvent& e)
    {
        if (window.cursorState() == coffee::graphics::CursorState::Disabled) {
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(io.BackendPlatformUserData);

        if (e.entered) {
            backendData->windowPtr = &window;

            io.AddMousePosEvent(backendData->lastMousePos.x, backendData->lastMousePos.y);
        }
        else if (backendData->windowPtr == &window) {
            backendData->windowPtr = nullptr;
            backendData->lastMousePos = io.MousePos;

            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        }
    }

    void ImGuiImplementation::mouseClickCallback(const coffee::graphics::Window& window, const coffee::MouseClickEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl, e.control);
        io.AddKeyEvent(ImGuiMod_Shift, e.shift);
        io.AddKeyEvent(ImGuiMod_Alt, e.alt);
        io.AddKeyEvent(ImGuiMod_Super, e.super);

        uint8_t button = static_cast<uint8_t>(e.button);
        if (button >= 0 && button < ImGuiMouseButton_COUNT) {
            io.AddMouseButtonEvent(button, e.state == coffee::State::Press);
        }
    }

    void ImGuiImplementation::mousePositionCallback(const coffee::graphics::Window& window, const coffee::MouseMoveEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(io.BackendPlatformUserData);

        float xPosition = e.x;
        float yPosition = e.y;

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            VkOffset2D windowPosition = window.windowPosition();
            xPosition += windowPosition.x;
            yPosition += windowPosition.y;
        }

        io.AddMousePosEvent(xPosition, yPosition);
        backendData->lastMousePos = { xPosition, yPosition };
    }

    void ImGuiImplementation::mouseWheelCallback(const coffee::graphics::Window& window, const coffee::MouseWheelEvent& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(e.x, e.y);
    }

    void ImGuiImplementation::keyCallback(const coffee::graphics::Window& window, const coffee::KeyEvent& e)
    {
        if (e.state != coffee::State::Press && e.state != coffee::State::Release) {
            return;
        }

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl, e.control);
        io.AddKeyEvent(ImGuiMod_Shift, e.shift);
        io.AddKeyEvent(ImGuiMod_Alt, e.alt);
        io.AddKeyEvent(ImGuiMod_Super, e.super);

        ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        backendData->keyOwnerWindows[static_cast<uint32_t>(e.key)] = (e.state == coffee::State::Press) ? &window : nullptr;

        ImGuiKey key = keyToImGui(e.key);
        io.AddKeyEvent(key, e.state == coffee::State::Press);
        io.SetKeyEventNativeData(key, static_cast<int>(e.key), static_cast<int>(e.scancode));
    }

    void ImGuiImplementation::charCallback(const coffee::graphics::Window& window, char32_t ch)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(ch);
    }

    void ImGuiImplementation::updateMonitors()
    {
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        platformIO.Monitors.resize(0);

        for (const auto& monitor : coffee::graphics::Monitor::monitors()) {
            ImGuiPlatformMonitor imGuiMonitor{};

            VkExtent2D position = monitor->position();
            imGuiMonitor.MainPos = imGuiMonitor.WorkPos =
                ImVec2{ static_cast<float>(position.width), static_cast<float>(position.height) };

            coffee::graphics::VideoMode videoMode = monitor->currentVideoMode();
            imGuiMonitor.MainSize = imGuiMonitor.WorkSize =
                ImVec2{ static_cast<float>(videoMode.width), static_cast<float>(videoMode.height) };

            VkRect2D workArea = monitor->workArea();
            if (workArea.extent.width > 0 && workArea.extent.height > 0) {
                imGuiMonitor.WorkPos = ImVec2{ static_cast<float>(workArea.offset.x), static_cast<float>(workArea.offset.y) };
                imGuiMonitor.WorkSize = ImVec2{ static_cast<float>(workArea.extent.width), static_cast<float>(workArea.extent.height) };
            }

            coffee::Float2D scale = monitor->contentScale();
            imGuiMonitor.DpiScale = scale.x;

            platformIO.Monitors.push_back(imGuiMonitor);
        }
    }

    void ImGuiImplementation::updateMouse(const coffee::graphics::WindowPtr& applicationWindow)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(io.BackendPlatformUserData);

        if (backendData->fullControlWindowPtr != nullptr) {
            if (!backendData->fullControlWindowPtr->isButtonPressed(coffee::Keys::Escape)) {
                io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
                return;
            }

            backendData->fullControlWindowPtr = nullptr;
        }

        ImGuiID mouseViewportID = 0;
        const ImVec2 previousMousePosition = io.MousePos;

        for (int32_t i = 0; i < platformIO.Viewports.Size; i++) {
            ImGuiViewport* viewport = platformIO.Viewports[i];
            coffee::graphics::Window* window = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;

            if (window->isFocused()) {
                if (io.WantSetMousePos) {
                    window->setMousePosition({ previousMousePosition.x - viewport->Pos.x, previousMousePosition.y - viewport->Pos.y });
                }

                if (backendData->windowPtr == nullptr) {
                    coffee::Float2D mousePosition = window->mousePosition();

                    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                        VkOffset2D windowPosition = window->windowPosition();
                        mousePosition.x += windowPosition.x;
                        mousePosition.y += windowPosition.y;
                    }

                    backendData->lastMousePos = ImVec2{ mousePosition.x, mousePosition.y };
                    io.AddMousePosEvent(mousePosition.x, mousePosition.y);
                }
            }

            bool windowNoInput = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;

            windowNoInput ? window->enablePassthrough() : window->disablePassthrough();

            if (window->isFocused() && !windowNoInput) {
                mouseViewportID = viewport->ID;
            }
        }

        if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport) {
            io.AddMouseViewportEvent(mouseViewportID);
        }
    }

    void ImGuiImplementation::updateCursor(const coffee::graphics::WindowPtr& applicationWindow)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (static_cast<ImGuiBackendPlatformData*>(io.BackendPlatformUserData)->fullControlWindowPtr != nullptr) {
            return;
        }

        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
        ImGuiBackendPlatformData* backendData = static_cast<ImGuiBackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

        for (int32_t i = 0; i < platformIO.Viewports.Size; i++) {
            ImGuiViewport* viewport = platformIO.Viewports[i];
            coffee::graphics::Window* window = static_cast<ImGuiViewportData*>(viewport->PlatformUserData)->windowHandle;

            if (cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
                window->hideCursor();
                continue;
            }

            window->setCursor(backendData->cursors[cursor]);
            window->showCursor();
        }
    }

} // namespace yorna