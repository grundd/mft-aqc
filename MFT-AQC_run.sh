# shell script must be first allowed: chmod +x aQC_selectedRunList.sh
#!/bin/bash
# to run it do (inside ali shell):
# ./MFT-AQC_run.sh

declare -a arr=(
    # 2022:
    # apass2
        # '2022/LHC22p_O2-3529'
        # '2022/LHC22q_O2-3548' 
    # apass3
        # '2022/LHC22p_O2-3573'
        # '2022/LHC22q_O2-3569'
    # 2023:
        # '2023/544475+544476'
    # cpass0:
        # '2023/LHC23zo_10kHz_O2-3966'
        # '2023/LHC23zo_500kHz_O2-3966'
        # '2023/LHC23zp_10kHz_O2-3966'
        # '2023/LHC23zp_500kHz_O2-3966'
        # '2023/LHC23zx_O2-4181'
        # '2023/LHC23zy_O2-4182'
        # '2023/LHC23zz_O2-4190'
        # '2023/LHC23zza_O2-4199'
        # '2023/LHC23zzb_O2-4201'
        # '2023/LHC23zzc_O2-4206'
        # '2023/LHC23zzd_O2-4207'
        # '2023/LHC23zze_O2-4219'
        # '2023/LHC23zzf_O2-4228'
        # '2023/LHC23zzg_O2-4228'
        # '2023/LHC23zzh_O2-4231'
        # '2023/LHC23zzi_O2-4231'
        #  LHC23zzj -> VdM scans
        # '2023/LHC23zzk_O2-4250'
        # '2023/LHC23zzl_O2-4262'
        # '2023/LHC23zzm_O2-4263'
        # '2023/LHC23zzn_O2-4263'
        # '2023/LHC23zzo_O2-4310_01'
        # '2023/LHC23zzo_O2-4310_02'
    # cpass1, cpass2, ...:
        # '2023/LHC23zzh_O2-4231_cpass0to6+10'
    # relVal:
        # '2023/LHC23zzk_544508_relval'
    # apass1:
        # '2023/LHC23f_O2-4443'
        # '2023/LHC23g_O2-4449'
        # '2023/LHC23h_O2-4423'
        # '2023/LHC23i_O2-4424'
        # '2023/LHC23j_O2-4424'
        # '2023/LHC23k_O2-4424'
        # '2023/LHC23l_O2-4399'
        # '2023/LHC23m_O2-4399'
        # '2023/LHC23n_O2-4452'
        # '2023/LHC23o_O2-4413'
        # '2023/LHC23p_O2-4413'
        # '2023/LHC23q_O2-4413'
        # '2023/LHC23r_O2-4413'
        # '2023/LHC23s_O2-4397'
        # '2023/LHC23t_O2-4462'
        # '2023/LHC23u_O2-4395'
        # '2023/LHC23v_O2-4395'
        # '2023/LHC23w_O2-4462'
        # '2023/LHC23y_O2-4406'
        # '2023/LHC23z_O2-4396'
        # '2023/LHC23za_O2-4396'
        # '2023/LHC23zb_O2-4462'
        # '2023/LHC23zc_O2-4398'
        # '2023/LHC23zd_O2-4398'
        # '2023/LHC23ze_O2-4405'
        # '2023/LHC23zf_O2-4405'
        # '2023/LHC23zg_O2-4405'
        # '2023/LHC23zh_O2-4405'
        # '2023/LHC23zi_O2-4405'
        # '2023/LHC23zj_O2-4405'
        # '2023/LHC23zk_O2-4405'
        # '2023/LHC23zl_O2-4429'
        # '2023/LHC23zm_O2-4429'
        # '2023/LHC23zn_O2-4429'
        # '2023/LHC23zo_10kHz_O2-4173'
        # '2023/LHC23zo_500kHz_O2-4173'
        # '2023/LHC23zp_10kHz_O2-4173'
        # '2023/LHC23zp_500kHz_O2-4173'
        # '2023/LHC23zq_O2-4402'
        # '2023/LHC23zr_O2-4402'
        # '2023/LHC23zs_O2-4402'
        # '2023/LHC23zt_O2-4506'
        # '2023/LHC23zzf_O2-4526'
        # '2023/LHC23zzg_O2-4526'
        # '2023/LHC23zzh_O2-4527'
        # '2023/LHC23zzi_O2-4527'
        # '2023/LHC23zzk_O2-4545'
    # newer apasses:
        # '2023/LHC23zs_O2-4563'
        # '2023/LHC23zzf_O2-4574'
        # '2023/LHC23zzg_O2-4574'
        # '2023/LHC23zzh_O2-4575'
        # '2023/LHC23zzi_O2-4575'
        # '2023/LHC23zzk_O2-4576'
    # IR groups:
        # 'PbPb_IR_groups/0-10kHz'
        # 'PbPb_IR_groups/10-15kHz'
        # 'PbPb_IR_groups/15-20kHz'
        # 'PbPb_IR_groups/20-25kHz'
        # 'PbPb_IR_groups/25-30kHz'
        # 'PbPb_IR_groups/30-45kHz'
    # MC
        # '2023/MC_LHC23j5_vs_LHC23zo_apass1'
        # '2023/MC_LHC23j5_vs_LHC23zp_apass1'
        # '2023/MC_LHC23k4b_vs_LHC23pp_apass1'
        # '2023/MC_LHC23k4b_vs_LHC23pp_apass1_bkg'
        # '2023/MC_LHC23k6c_vs_LHC23PbPb_apass1'
        # '2023/MC_LHC23k6d_vs_LHC23PbPb_apass2'
)

do_aqc () {
    root -q 'downloadQCObjects.cxx("'input/runLists/$1.txt'")'
    root -q -b 'plotQCObjects.cxx("'input/runLists/$1.txt'")'
    root -q 'prepareSlides.cxx("'input/runLists/$1.txt'")'
}

# do the quality checks
for i in "${arr[@]}"
do
   echo "Input dataset: $i"
   do_aqc $i
done