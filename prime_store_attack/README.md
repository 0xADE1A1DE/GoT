# Preparation

## GnuPG

Download gnupg-1.4.13

Apply the following diff:

```diff
diff --git a/g10/gpg.c b/g10/gpg.c
index 96f9086..1f7af22 100644
--- a/g10/gpg.c
+++ b/g10/gpg.c
@@ -83,6 +83,7 @@ enum cmd_and_opt_values
     aDetachedSign = 'b',
     aSym         = 'c',
     aDecrypt     = 'd',
+    aTEST        = 'p',
     aEncr        = 'e',
     oInteractive  = 'i',
     oKOption     = 'k',
@@ -392,6 +393,7 @@ static ARGPARSE_OPTS opts[] = {
     { aSym, "symmetric", 256, N_("encryption only with symmetric cipher")},
     { aStore, "store",     256, "@"},
     { aDecrypt, "decrypt",   256, N_("decrypt data (default)")},
+    { aTEST, "test",   256, N_("Test")},
     { aDecryptFiles, "decrypt-files", 256, "@"},
     { aVerify, "verify"   , 256, N_("verify a signature")},
     { aVerifyFiles, "verify-files" , 256, "@" },
@@ -1634,6 +1636,12 @@ collapse_args(int argc,char *argv[])
   return str;
 }

+uint64_t __always_inline time_in_100us() {
+    struct timeval te;
+    gettimeofday(&te, NULL);
+    return te.tv_sec*10000LL + te.tv_usec/100LL;
+}
+
 static void
 parse_trust_model(const char *model)
 {
@@ -2098,6 +2106,7 @@ main (int argc, char **argv )

          case aDecryptFiles: multifile=1; /* fall through */
          case aDecrypt: set_cmd( &cmd, aDecrypt); break;
+    case aTEST: set_cmd( &cmd, aTEST); break;

          case aEncrFiles: multifile=1; /* fall through */
          case aEncr: set_cmd( &cmd, aEncr); break;
@@ -3491,6 +3500,30 @@ main (int argc, char **argv )
          }
        break;

+      case aTEST:
+      {
+        MPI res = mpi_alloc(mpi_nlimb_hint_from_nbits (2048));
+        MPI base = mpi_alloc(mpi_nlimb_hint_from_nbits (2048));
+        MPI dp = mpi_alloc(mpi_nlimb_hint_from_nbits (2048));
+        MPI p = mpi_alloc(mpi_nlimb_hint_from_nbits (2048));
+
+        mpi_fromstr(base, "0x9066f9783a432d3ab097fcc7e84c48eb17970775a5db2f320606481016525462ce94860d21b65781152f2d94e565ddb3a4ded48b621d2726cb2e5fac1598a83a956c895cbf10726c365bed54530b6a806761b971cb7eab442f7e5ee327798bb2a644a9716afedda0eed639f43f54c44450ec7946bbbba91561c55cb2b7dc32dc1fe343c0b9005de93098fb322ce71242c84d56d8602907aa28437a884057c116876ac547bb2eda7176f4d51a8d1eeea9c648f9377ea9c02b63a5f70c3a786532e7b9fc3fc6d2f56be51895707e15c11cff8e386713da710ae9b37f12b400c1b4a8494a4e9085ff6499afd08a21614c2036fd41be58f8822bb3a090bfc68d2766b38b4c220cb32b63e365389596b7f00e152b053ea062d87731f463d0ff6b6c1fe7ec94314cf35a1210a730155e102171818983c7df43a616e6589430cb9d810afd33ebf69c06b345c5981f8e4fc49a2542b318093c8d391c04cecc964456520166f6fc90ed693ce0b04dd29f16a9aa4032d4f28c747f6409e035633ef28161d56a899a444b275279fe66656e5a9d20f23adf3b3dfa76db753377acf2cb6babc635219dd8ed951eb4b7899b521705e435c7c970549aa602a2ae0deba11a5dc2a4cc683c372df2d87a84431a04cfab08ae11135b74c7dece115f0c781138d19a0376519f02cc5a49e7de57697570aa1268170064877201b3a03b82ceed1ac90e1e");
+        mpi_fromstr(dp, "0x17f6e67ced6d5f2bb7bc5a362dade0664827edd4812ced037c703da7a140376f6b864ea02b5a0ce0f569def2c43317061e258b0cd93442c7a5c");
+        mpi_fromstr(p, "0x96fa22c5b2ef276e56d023f9ff369139a583e1b91efc070290d63394d292764e5bbbf111d35ffd2e9d2d14b507c8900bbb099f5b57318df366214b9da5ea65bd88de6324415f4f4fe94c769c5ce33f19e1af1140169a248d8b58f3904c23a07f31e6521177cc075f3fac3ee536c50d6d213f7ec8e3e417ae04129b34ff612d31d2109e6fdddbe196b9963960d2baeb3051f929790b53ab503f5229597f3ee7dfbe5877e587c7701ae466d5568e7fa7fb00cf82e12ea622f419eee5848cff131f57a8b01778fcc7843abf60b3d80df60af6a42aeb0c3a11686f2086537e98225fe3d794239e11d6d91e92146db82a925fd0f569c1f9eb71bcfa807c4defc0b11544d2c6e9635bdd69f530dcae876918e332fd2dfd55d1c8ff1f8a4128a347fb65bda7fa63a48de870a9d011879c4b492658dc647340309b9411b9ab0d055067460c32371f396fb90eeaf74e014767ea752d23803751ecf44043acf89e8e1c7441ac0be1ab0c56c19cd703ac379b4e09f7e2d09445ef0f30397c51e676c849c28093b329168b8d535c585769caaaaf158d72248e0162568fc92c2f972bd22e2f95a4b243cdd6877dbabf53307adc5991b7f6ee19c44141fb4c74361f2d7ddda2ba50e36bb83dcde1055b1e2e027eb954068224c190f9a255f788810cffb2c91a5b8280865b59795615241cb81400bf1a4e18592788085313749d6eaeaf103165bf");
+
+        sleep(1);
+
+        while(1) {
+          uint64_t start = time_in_100us();
+          mpi_powm(res, base, dp, p);
+          uint64_t end = time_in_100us();
+          printf("%ld\n", end-start);
+          usleep(1000);
+        }
+      }
+
+  break;
+
       case aSignKey:
        if( argc != 1 )
          wrong_args(_("--sign-key user-id"));
diff --git a/mpi/mpih-mul.c b/mpi/mpih-mul.c
index 9cb25ea..801d288 100644
--- a/mpi/mpih-mul.c
+++ b/mpi/mpih-mul.c
@@ -265,7 +265,7 @@ mpih_sqr_n_basecase( mpi_ptr_t prodp, mpi_ptr_t up, mpi_size_t size )
 }


-void
+void __attribute__ ((aligned (0x80)))
 mpih_sqr_n( mpi_ptr_t prodp, mpi_ptr_t up, mpi_size_t size, mpi_ptr_t tspace)
 {
     if( size & 1 ) {
```

- This diff introduces a debug flag to gnpug, which calls `mpi_powm` in an infinite loop.
- It also aligns `mpih_sqr_n` to (double) cache line boundry.

Modify the path in `~/GoT/prime_store_attack/src/experiments/prime_probe_gnupg.c`. There are 3 instances of an hardcoded path, namely `/home/acrypto/Documents/bad_gpg/gpg_random_457_looping`, modify them to the path of your compilation. Within our machine, the path included works and can be left as is.

## Prime+Store

```bash
# building
cd ~/GoT/prime_store_attack/src/
rm -rf ../build
mkdir ../build
make all
rm -rf ~/prime_store
cp -r ../build ~/prime_store
```

# Basic Test

We expect that at `~/prime_store`, there would be a binary `main`

It should output:

```
Usage: /home/usenix/prime_store/main [experiment]

             prime_probe_gnupg          Prime probe an old, slightly modifed, gnupg (v1.4.13) known for non-constant time exponentiation.

            test_amplification          Test the tree amplification.

                    test_gates          Test the various gates.

                test_retention          Test schemes of speculative bit retention over time.
```

# Evaluation workflow

## Prime+Store:

```bash
sudo ~/prime_store/main prime_probe_gnupg > /tmp/prime_store_traces.txt
```

We consider `prime_store_traces.txt` to be the artifact of this experiment, gathering the traces take approximately 10 hours.