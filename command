python3 miniob_test.py \
        --test-case-dir=./test  \
        --test-case-scores=case-scores.json \
        --test-result-dir=result \
        --test-result-tmp-dir=./result_tmp \
        --test-cases=primary-drop-table \
        --db-base-dir=/home/ubuntu/DS/oceanbase/miniob-2022/build \
        --db-config=/home/ubuntu/DS/oceanbase/miniob-2022/etc/observer.ini \
        --code-type = none \
        --server-started \
        --debug \
        --log=stdout

# primary-select-meta
python3 miniob_test.py \
        --test-case-dir=./test  \
        --test-case-scores=case-scores.json \
        --test-result-dir=result \
        --test-result-tmp-dir=./result_tmp \
        --test-cases=primary-select-meta \
        --db-base-dir=/home/ubuntu/DS/oceanbase/miniob-2022/build \
        --db-config=/home/ubuntu/DS/oceanbase/miniob-2022/etc/observer.ini \
        --code-type = none \
        --debug \
        --log=stdout


python3 miniob_test.py \
        --test-case-dir=./test  \
        --test-case-scores=case-scores.json \
        --test-result-dir=result \
        --test-result-tmp-dir=./result_tmp \
        --test-cases=primary-update \
        --db-base-dir=/home/ubuntu/DS/oceanbase/miniob-2022/build \
        --db-config=/home/ubuntu/DS/oceanbase/miniob-2022/etc/observer.ini \
        --code-type = none \
        --debug \
        --log=stdout

