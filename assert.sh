#usr/bin/bash
echo "Asserting Sequential version..."
difference="$(diff sequential_matrix.txt matrix_original.txt)"
[[ ! -z "$difference" ]] && echo "NOK" || echo "OK"