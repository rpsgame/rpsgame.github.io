/* 
 * @requires Vector3
 * @requires Color
 */

CheckerMaterial = function(scale, reflectiveness) { this.scale = scale; this.reflectiveness = reflectiveness; };

CheckerMaterial.prototype = {
    sample : function(ray, position, normal) {
        return Math.abs((Math.floor(position.x * 0.1) + Math.floor(position.z * this.scale)) % 2) < 1 ? Color.black : Color.white;
    }
};