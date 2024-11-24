
// Arrays:
if (!Array.firstOrDefault)
{
	Object.defineProperty(Array.prototype, 'firstOrDefault', {
		value: function(predicate) 
		{
			var ret = null;
			
			if (predicate != null && predicate != undefined)
			{
				for (var i in this)
				{
					if (predicate(this[i]) == true)
					{
						ret = this[i];
						break;
					}
				}
			}

			return ret;
		}
	});
}
