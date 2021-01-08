function annimation(A, B, h)

    [m1,n1] = size(A);
    [m2,n2] = size(B);
    
    if n1 ~= n2 || n1 ~= 3 || mod(m1, 3) ~= 0 || mod(m2, 3) ~= 0
        return;
    end
    
    for script_idx = 0:m1/3-1
        row_start_1 = script_idx*3 + 1;
        row_end_1 = script_idx*3 + 3;
        row_start_2 = (script_idx+1)*3 + 1;
        row_end_2 = (script_idx+1)*3 + 3;
        
        script_A_1 = A(row_start_1:row_end_1, :);
        script_A_2 = A(row_start_2:row_end_2, :);
        script_B_1 = B(row_start_1:row_end_1, :);
        script_B_2 = B(row_start_2:row_end_2, :);
    end
    
    origin_pts = [0,0,1; 1,0,1; 1,1,1; 0,1,1];
    figure(1); axis equal; hold on;
    figure(1); plot([origin_pts(:,1)', origin_pts(1,1)], [origin_pts(:,2)', origin_pts(1,2)]*h, 'r+-');
    for i=0:10
        tt = i*0.1*B + (1-i*0.1)*A;
        dest_pts = tt*(origin_pts');
        dest_pts = dest_pts';
        
        [r,~] = size(dest_pts);
        for j=1:r
            dest_pts(j, :) = dest_pts(j, :)/dest_pts(j, 3);
        end
        
        figure(1); plot([dest_pts(:,1)', dest_pts(1,1)], [dest_pts(:,2)', dest_pts(1,2)]*h, 'b-.');
        dest_pts
        pause(1);
    end
end