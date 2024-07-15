<!DOCTYPE html>
<html lang="en">
    <head>
        <title>ALICE MFT Asynchronous QC</title>
        <link rel="stylesheet" href="css/styles.css">
        <base target="_blank" rel="noopener noreferrer"> 
    </head>
    <body>
        <h1>ALICE MFT Asynchronous QC</h1>
        <p><a href="https://docs.google.com/spreadsheets/d/1h16tgJ4k90bdZ45qB3fpbw9DqZPEyc02FR2wl0bckyE/edit?usp=sharing">MFT A-QC Google Sheet</a></p>
        <p>QC Summary Tables (RCTs): 
        <a href="https://docs.google.com/spreadsheets/d/1ZI8RWtJfl9le0GdtaQ3lzJwcqgYPiWSAc9Md4hvkxTk/">2024 pp</a>, 
        <a href="https://docs.google.com/spreadsheets/d/1vsl-pkLdUoNXZm6muPyMENWdDSKM7UDtQLcXc2B9QhE/">2023 PbPb</a>, 
        <a href="https://docs.google.com/spreadsheets/d/1YBQLXWwwc3aC3B_PYVpFkTgEP0n6u1ovtYfiCOMWnTc/">2023 pp</a>, 
        <a href="https://docs.google.com/spreadsheets/d/14vXFYVx3oVE8wgJKmIBhjvAt6NpGp7D6H4AmBM9E0Cw/">2022</a></p>

        <h2>2024 pp</h2>
        <?php include "./2024_pp.html" ?>

        <h2>2023 Pb+Pb</h2>
        <?php include "./2023_PbPb.html" ?>

        <h2>2023 pp</h2>
        <?php include "./2023_pp.html" ?>

        <h2>2023: extra reports</h2>
        <ul>
            <li><a href="reports/LHC23zs_apass2_vs_1.pdf">LHC23zs_apass2_vs_1.pdf</a></li>
            <li><a href="reports/LHC23zzh_cpass0to6+10.pdf">LHC23zzh_cpass0to6+10.pdf</a></li>
        </ul>

        <h2>2022</h2>
        <ul>
            <li>LHC22f: <a href="reports/LHC22f_apass4.pdf">apass4.pdf</a></li>
            <li>LHC22o: <a href="reports/LHC22o_apass6.pdf">apass6.pdf</a>, <a href="reports/LHC22o_apass7_skimmed.pdf">apass7_skimmed.pdf</a></li>
            <li>LHC22p: <a href="reports/LHC22p_apass2.pdf">apass2.pdf</a>, <a href="reports/LHC22p_apass3.pdf">apass3.pdf</a>, <a href="reports/LHC22p_apass7_skimmed.pdf">apass7_skimmed.pdf</a></li>
            <li>LHC22q: <a href="reports/LHC22q_apass2.pdf">apass2.pdf</a>, <a href="reports/LHC22q_apass3.pdf">apass3.pdf</a></li>
            <li>LHC22r: <a href="reports/LHC22r_apass7_skimmed.pdf">apass7_skimmed.pdf</a></li>
            <li>LHC22s: <a href="reports/LHC22s_apass5.pdf">apass5.pdf</a></li>
            <li>LHC22t: <a href="reports/LHC22t_apass7_skimmed.pdf">apass7_skimmed.pdf</a></li>
        </ul>

        <h2>MC productions</h2>
        <?php include "./MC.html" ?>
    </body>
</html>
