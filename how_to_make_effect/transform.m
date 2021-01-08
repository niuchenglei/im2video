
function transform(A, B, h, w)

    [m1,n1] = size(A);
    [m2,n2] = size(B);
    
    if m1 ~= m2 || m1 ~= 3 || n1 ~= n2 || n1 ~= 3
        return;
    end
    
    origin_pts = [0,0,1; 1,0,1; 1,1,1; 0,1,1];
    figure(1); axis ij; axis equal; hold on;
    figure(1); plot([origin_pts(:,1)', origin_pts(1,1)].*w, [origin_pts(:,2)', origin_pts(1,2)].*h, 'r+-');
    for i=0:10
        tt = i*0.1*B + (1-i*0.1)*A;
        dest_pts = tt*(origin_pts');
        dest_pts = dest_pts';
        
        [r,~] = size(dest_pts);
        for j=1:r
            dest_pts(j, :) = dest_pts(j, :)/dest_pts(j, 3);
        end
        
        figure(1); plot([dest_pts(:,1)', dest_pts(1,1)].*w, [dest_pts(:,2)', dest_pts(1,2)].*h, 'b-.');
        dest_pts
        pause(1);
    end
end
